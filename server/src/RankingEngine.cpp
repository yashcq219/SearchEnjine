#include "../include/RankingEngine.h"
#include <iostream>

namespace searchengine {

RankingEngine::RankingEngine() {}

RankingEngine::~RankingEngine() {}

/**
 * Removes duplicate words from the query.
 *  The dynamic array of query words.
 *  returns  A dynamic array of unique query words.
 */
DynamicArray<std::string> RankingEngine::deduplicateQueryWords(const DynamicArray<std::string>& queryWords) const {
    DynamicArray<std::string> uniqueWords;
    HashMap<std::string, bool> seenMap;

    int totalWords = const_cast<DynamicArray<std::string>&>(queryWords).getSize();
    for (int i = 0; i < totalWords; ++i) {
        const std::string& word = queryWords.get(i);

        if (word.empty()) {
            continue;
        }

        if (!seenMap.exist(word)) {
            seenMap.set(word, true);
            uniqueWords.append(word);
        }
    }

    return uniqueWords;
}
// sort acc. to query word match count, occurence and url
void RankingEngine::sortCandidatePages(DynamicArray<PageScore>& candidates) const {
    int listSize = candidates.getSize();

    // Perform Insertion Sort on candidate PageScore list
    for (int i = 1; i < listSize; ++i) {
        PageScore key = candidates.get(i);
        int j = i - 1;

        while (j >= 0) {
            const PageScore& current = candidates.get(j);
            bool shouldShift = false;

            // Rule 1: Higher matched query word count comes first (all N words -> N-1 -> N-2 -> ...)
            if (current.matchedCount < key.matchedCount) {
                shouldShift = true;
            }
            // Rule 2: Equal matched count -> Higher total cumulative occurrence sum comes first
            else if (current.matchedCount == key.matchedCount &&
                     current.totalOccurrence < key.totalOccurrence) {
                shouldShift = true;
            }
            // Rule 3: Equal matched count & total occurrence -> Sort alphabetically by URL
            else if (current.matchedCount == key.matchedCount &&
                     current.totalOccurrence == key.totalOccurrence &&
                     current.url > key.url) {
                shouldShift = true;
            }

            if (shouldShift) {
                candidates.get(j + 1) = current;
                j--;
            } else {
                break;
            }
        }

        candidates.get(j + 1) = key;
    }
}

DynamicArray<PageScore> RankingEngine::rankQueryDetailed(const DynamicArray<std::string>& queryWords,
                                                       const HashMap<std::string, DynamicArray<Posting>>& index) const {
    DynamicArray<PageScore> rankedResults;

    // Step 1: Clean and deduplicate query words
    DynamicArray<std::string> cleanWords = deduplicateQueryWords(queryWords);
    int wordCount = cleanWords.getSize();

    if (wordCount == 0 || const_cast<HashMap<std::string, DynamicArray<Posting>>&>(index).empty()) {
        return rankedResults;
    }

    // Step 2: Accumulate page matches across query words
    HashMap<std::string, PageScore> candidateScoreMap;
    DynamicArray<std::string> candidateUrlList;

    for (int i = 0; i < wordCount; ++i) {
        std::string word = cleanWords.get(i);

        // Check if word exists in the inverted index HashMap
        if (!index.exist(word)) {
            continue;
        }

        // Fetch top postings list for this word from the HashMap
        const DynamicArray<Posting>& wordPostings = index.get(word);
        int postingCount = const_cast<DynamicArray<Posting>&>(wordPostings).getSize();

        for (int j = 0; j < postingCount; ++j) {
            const Posting& p = wordPostings.get(j);

            // Update existing candidate page score or register new page candidate
            if (candidateScoreMap.exist(p.url)) {
                PageScore existing = candidateScoreMap.get(p.url);
                existing.matchedCount += 1;
                existing.totalOccurrence += p.occurrence;
                candidateScoreMap.set(p.url, existing);
            } else {
                PageScore newScore(p.url, 1, p.occurrence);
                candidateScoreMap.set(p.url, newScore);
                candidateUrlList.append(p.url);
            }
        }
    }

    // Step 3: Populate array of candidate PageScore entries
    DynamicArray<PageScore> candidateList;
    int candidateCount = candidateUrlList.getSize();
    for (int i = 0; i < candidateCount; ++i) {
        std::string url = candidateUrlList.get(i);
        candidateList.append(candidateScoreMap.get(url));
    }

    // Step 4: Sort candidate pages using multi-tier ranking hierarchy
    sortCandidatePages(candidateList);

    // Step 5: Retain at most m_maxResults (top 10) results
    int finalCount = (candidateList.getSize() < m_maxResults) ? candidateList.getSize() : m_maxResults;
    for (int i = 0; i < finalCount; ++i) {
        rankedResults.append(candidateList.get(i));
    }

    return rankedResults;
}

DynamicArray<std::string> RankingEngine::rankQuery(const DynamicArray<std::string>& queryWords,
                                                  const HashMap<std::string, DynamicArray<Posting>>& index) const {
    DynamicArray<std::string> topUrls;

    DynamicArray<PageScore> detailedResults = rankQueryDetailed(queryWords, index);

    int count = detailedResults.getSize();
    for (int i = 0; i < count; ++i) {
        topUrls.append(detailedResults.get(i).url);
    }

    return topUrls;
}

int RankingEngine::getMaxResults() const {
    return m_maxResults;
}

} // namespace searchengine
