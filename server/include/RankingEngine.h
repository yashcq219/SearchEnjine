#ifndef RANKING_ENGINE_H
#define RANKING_ENGINE_H

#include <string>
#include "IndexLoader.h"
#include "../externals/ds_library/include/SuperCodersCollections.h"

namespace searchengine {

struct PageScore {
    std::string url;
    int matchedCount;
    int totalOccurrence;

    PageScore() : url(""), matchedCount(0), totalOccurrence(0) {}
    PageScore(const std::string& u, int count, int occ)
        : url(u), matchedCount(count), totalOccurrence(occ) {}
};

class RankingEngine {
private:
    int m_maxResults = 10;

    void sortCandidatePages(DynamicArray<PageScore>& candidates) const;
    
    DynamicArray<std::string> deduplicateQueryWords(const DynamicArray<std::string>& queryWords) const;

public:
    RankingEngine();
    ~RankingEngine();

    DynamicArray<std::string> rankQuery(const DynamicArray<std::string>& queryWords,
                                        const HashMap<std::string, DynamicArray<Posting>>& index) const;

    DynamicArray<PageScore> rankQueryDetailed(const DynamicArray<std::string>& queryWords,
                                              const HashMap<std::string, DynamicArray<Posting>>& index) const;

    int getMaxResults() const;
};

} 

#endif // RANKING_ENGINE_H
