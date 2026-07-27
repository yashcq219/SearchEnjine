#ifndef INDEX_LOADER_H
#define INDEX_LOADER_H

#include <string>
#include "../externals/ds_library/include/SuperCodersCollections.h"

namespace searchengine {

/**
 * Contains document URL and occurrence frequency count.
 * Note: Does not use urlId; direct url string is used.
 */
struct Posting {
    std::string url;
    int occurrence;

    Posting() : url(""), occurrence(0) {}
    Posting(const std::string& u, int occ) : url(u), occurrence(occ) {}
};

/**
 
 * Retains only top K URLs with the highest occurrence frequency per word in descending order.
 */
class IndexLoader {
private:
    HashMap<std::string, DynamicArray<Posting>> m_index;
    int m_maxUrlsPerWord = 10;
    bool m_isLoaded;

    
    DynamicArray<Posting> selectTopKPostings(const DynamicArray<Posting>& rawPostings) const;

public:
    
     IndexLoader();
    
    ~IndexLoader();

    bool loadFromDatabase(const std::string& dbPath);

    
    bool hasWord(const std::string& word) const;

    
    DynamicArray<Posting> getPostings(const std::string& word) const;

    
    const HashMap<std::string, DynamicArray<Posting>>& getIndex() const;

   
    int getLoadedWordCount() const;

   
    bool isLoaded() const;
     


    int getMaxUrlsPerWord() const;

    void clear();
};

} // namespace searchengine

#endif // INDEX_LOADER_H
