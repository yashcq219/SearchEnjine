- Date: Project 03A, Day 1 (Session 3)
- Duration: 2 hours
- Goal:
    Implement the IndexLoader component in C++ to load the persistent SQLite database into an in-memory inverted index map using custom ds_library collections.
- Problem:
    The Search Engine requires loading the index database created by the Indexer into RAM at server startup, retaining only the Top 10 highest-frequency URLs per word (using direct URL strings and occurrence counts without urlId), deduplicating postings, and strictly using custom HashMap and DynamicArray data structures from the ds_library git submodule instead of built-in STL containers.
- What I Tried:
    - Declared struct Posting (containing url string and occurrence frequency) and class IndexLoader in server/include/IndexLoader.h.
    - Implemented database reading using SQLite 3 (sqlite3_open_v2, sqlite3_step) in server/src/IndexLoader.cpp with fallback schema support (inverted_index and postings tables).
    - Built a deduplication and insertion-sort algorithm operating directly on DynamicArray<Posting> to select and rank top 10 URLs per word in descending frequency order.
    - Eliminated all STL containers (std::vector, std::unordered_map) in favor of custom HashMap and DynamicArray from ds_library.
    - Created unit tests in server/tests/test_index_loader.cpp and updated Section 1 (Public API Table) and Section 4 (Public Methods Complexity Table) in docs/DesignProposal/designProposalV1/indexLoader.md.
- Outcome:
    - IndexLoader.h and IndexLoader.cpp fully implemented, documented, and maintainable.
    - Inverted index database successfully loads into memory at startup, retaining top 10 URLs per word in descending occurrence order.
    - Strictly adheres to custom ds_library collections requirement.
