- Date: Project 03A, Day 2 (Session 1)
- Duration: 2 hours
- Goal:
    Implement the RankingEngine component in C++ to process tokenized search query terms against the in-memory inverted index map and rank top matching web page URLs.
- Problem:
    The Search Engine needed a relevance ranking component that takes query terms (DynamicArray<std::string>) and the in-memory HashMap from IndexLoader, evaluates matching candidate pages across multi-tier priority rules (matched query word count first, cumulative occurrence frequency second, and alphabetical URL tie-break third), returns at most 10 URLs, and strictly relies on ds_library collections without standard STL containers.
- What I Tried:
    - Declared struct PageScore (storing url, matchedCount, and totalOccurrence) and class RankingEngine in server/include/RankingEngine.h.
    - Implemented query processing in server/src/RankingEngine.cpp: query term deduplication via HashMap<std::string, bool>, candidate page accumulation across query terms, and multi-tier Insertion Sort on DynamicArray<PageScore>.
    - Updated method signatures to accept const HashMap<std::string, DynamicArray<Posting>>& index directly as input from IndexLoader.
    - Verified functionality using unit tests in server/tests/test_ranking_engine.cpp to confirm that exact matches containing all N words rank higher than partial matches containing fewer words regardless of individual frequencies.
    - Cleaned up header code to keep method declarations concise and maintainable.
- Outcome:
    - RankingEngine.h and RankingEngine.cpp fully implemented and verified.
    - Successfully ranks search results according to matched word count, occurrence frequency, and alphabetical tie-breaking rules.
    - Component ready for integration with the Tokenizer and REST API Layer.
