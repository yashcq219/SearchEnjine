#include "../include/IndexLoader.h"
#include "../externals/sqlite/sqlite3.h"

#include <iostream>
#include <fstream>

namespace searchengine {

IndexLoader::IndexLoader() : m_isLoaded(false) {}

IndexLoader::~IndexLoader() {
    clear();
}

DynamicArray<Posting> IndexLoader::selectTopKPostings(const DynamicArray<Posting>& rawPostings) const {
    // 1. Deduplicate URLs and accumulate occurrence counts using custom HashMap & DynamicArray from ds_library
    HashMap<std::string, int> mergedMap;
    DynamicArray<std::string> uniqueUrls;
    int rawSize = const_cast<DynamicArray<Posting>&>(rawPostings).getSize();

    for (int i = 0; i < rawSize; ++i) {
        const Posting& p = rawPostings.get(i);
        if (p.url.empty() || p.occurrence <= 0) {
            continue;
        }

        if (mergedMap.exist(p.url)) {
            int currentOcc = mergedMap.get(p.url);
            mergedMap.set(p.url, currentOcc + p.occurrence);
        } else {
            mergedMap.set(p.url, p.occurrence);
            uniqueUrls.append(p.url);
        }
    }

    // 2. Build a merged DynamicArray of Postings
    DynamicArray<Posting> mergedList;
    int urlCount = uniqueUrls.getSize();
    for (int i = 0; i < urlCount; ++i) {
        std::string url = uniqueUrls.get(i);
        int occ = mergedMap.get(url);
        mergedList.append(Posting(url, occ));
    }

    // 3. Sort mergedList in descending order of occurrence (tie-break alphabetically by URL)
    // Using insertion sort on custom DynamicArray
    int listSize = mergedList.getSize();
    for (int i = 1; i < listSize; ++i) {
        Posting key = mergedList.get(i);
        int j = i - 1;

        while (j >= 0) {
            const Posting& current = mergedList.get(j);
            bool shouldShift = false;

            if (current.occurrence < key.occurrence) {
                shouldShift = true;
            } else if (current.occurrence == key.occurrence && current.url > key.url) {
                shouldShift = true;
            }

            if (shouldShift) {
                mergedList.get(j + 1) = current;
                j--;
            } else {
                break;
            }
        }
        mergedList.get(j + 1) = key;
    }

    // 4. Retain at most m_maxUrlsPerWord elements
    DynamicArray<Posting> topPostings;
    int limit = (listSize < m_maxUrlsPerWord) ? listSize : m_maxUrlsPerWord;
    for (int i = 0; i < limit; ++i) {
        topPostings.append(mergedList.get(i));
    }

    return topPostings;
}

bool IndexLoader::loadFromDatabase(const std::string& dbPath) {
    clear();

    // 1. Validate file existence
    std::ifstream dbCheck(dbPath);
    if (!dbCheck.good()) {
        std::cerr << "[IndexLoader] Error: Database file does not exist or corrupted at path: " << dbPath << std::endl;
        return false;
    }
    dbCheck.close();

    // 2. Open SQLite database in read-only mode
    sqlite3* db = nullptr;
    int rc = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "[IndexLoader] Error: Cannot open database: "
                  << (db ? sqlite3_errmsg(db) : "Unknown error") << std::endl;
        if (db) {
            sqlite3_close(db);
        }
        return false;
    }

    // 3. Prepare SQL query statement (with fallback schema support)
    sqlite3_stmt* stmt = nullptr;
    const char* queryPrimary = "SELECT word, url, occurrence FROM inverted_index;";
    const char* queryFallback1 = "SELECT word, url, frequency FROM inverted_index;";
    const char* queryFallback2 = "SELECT word, url, occurrence FROM postings;";

    rc = sqlite3_prepare_v2(db, queryPrimary, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        rc = sqlite3_prepare_v2(db, queryFallback1, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            rc = sqlite3_prepare_v2(db, queryFallback2, -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "[IndexLoader] Error: Failed to prepare SQL query on database tables. "
                          << sqlite3_errmsg(db) << std::endl;
                sqlite3_close(db);
                return false;
            }
        }
    }

    // 4. Temporary storage using ds_library HashMap & DynamicArray
    HashMap<std::string, DynamicArray<Posting>> tempWordMap;
    DynamicArray<std::string> uniqueWords;

    // 5. Read records sequentially from SQLite
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* wordCol = sqlite3_column_text(stmt, 0);
        const unsigned char* urlCol = sqlite3_column_text(stmt, 1);
        int occurrence = sqlite3_column_int(stmt, 2);

        if (!wordCol || !urlCol) {
            continue; // Skip invalid/corrupt null records
        }

        std::string word(reinterpret_cast<const char*>(wordCol));
        std::string url(reinterpret_cast<const char*>(urlCol));

        if (word.empty() || url.empty() || occurrence <= 0) {
            continue;
        }

        if (!tempWordMap.exist(word)) {
            tempWordMap.set(word, DynamicArray<Posting>());
            uniqueWords.append(word);
        }

        tempWordMap.get(word).append(Posting(url, occurrence));
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "[IndexLoader] Warning: Query execution ended unexpectedly: "
                  << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // 6. Select Top K URLs for each word and insert into in-memory inverted index map
    int wordCount = uniqueWords.getSize();
    for (int i = 0; i < wordCount; ++i) {
        std::string word = uniqueWords.get(i);
        const DynamicArray<Posting>& rawPostings = tempWordMap.get(word);

        DynamicArray<Posting> topPostings = selectTopKPostings(rawPostings);
        m_index.set(word, topPostings);
    }

    m_isLoaded = true;
    std::cout << "[IndexLoader] Successfully loaded " << m_index.getSize()
              << " unique words into in-memory inverted index." << std::endl;

    return true;
}

bool IndexLoader::hasWord(const std::string& word) const {
    if (!m_isLoaded || word.empty()) {
        return false;
    }
    return m_index.exist(word);
}

DynamicArray<Posting> IndexLoader::getPostings(const std::string& word) {
    if (!m_isLoaded || !m_index.exist(word)) {
        return nullptr;
    }
    DynamicArray<Posting> postings = m_index.get(word);
    return postings;
}

const HashMap<std::string, DynamicArray<Posting>>& IndexLoader::getIndex() const {
    return m_index;
}

int IndexLoader::getLoadedWordCount() const {
    return m_index.getSize();
}

bool IndexLoader::isLoaded() const {
    return m_isLoaded;
}

int IndexLoader::getMaxUrlsPerWord() const {
    return m_maxUrlsPerWord;
}

void IndexLoader::clear() {
    m_index.clear();
    m_isLoaded = false;
}

} // namespace searchengine
