- Date: Project 03A, Day 1 (Session 2)
- Duration: 1 hour
- Goal:
    Integrate external dependencies (SQLite 3 header/source files) and add Git submodules (ds_library and Crawler) to enable component reuse for custom data structures and the Tokenizer.
- Problem:
    Before starting C++ implementation, required third-party and shared dependencies needed to be linked into the project workspace—specifically SQLite 3 for reading the Indexer database, as well as the shared ds_library (for custom HashMap and DynamicArray data structures) and Crawler repository (for reusing identical Tokenizer logic) without duplicating code across repositories.
- What I Tried:
    - Verified and organized SQLite 3 amalgamation files (sqlite3.h, sqlite3.c, sqlite3ext.h) inside server/externals/sqlite/.
    - Configured and cloned Git submodule for ds_library from https://github.com/SuperCodersOrg/ds_library-Yash.git into server/externals/ds_library.
    - Configured and cloned Git submodule for Crawler from https://github.com/SuperCodersOrg/Crawler-YashCQ.git into server/externals/Crawler.
    - Verified .gitmodules tracking and staged submodule entries in Git.
- Outcome:
    - SQLite 3 dependency ready at server/externals/sqlite/.
    - Both ds_library and Crawler submodules successfully integrated under server/externals/.
    - Ready to begin backend C++ component development using shared data structures and Tokenizer logic.
