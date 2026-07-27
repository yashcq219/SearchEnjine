Date: Project 03A, Day 1
Duration: 2 hours
Goal:
Create a comprehensive Implementation Plan (Design Proposal V1) for the C++ Search Engine backend and REST API layer.
Problem:
Needed to design a modular C++ Search Engine architecture that eliminates database runtime lookup overhead by loading pre-computed index posting lists into memory during startup, while defining clean data flows and API contracts across IndexLoader, Tokenizer, RankingEngine, and REST API Layer.
What I Tried:
- Formulated the 4 primary decoupled components: IndexLoader (one-time DB memory loader), Tokenizer (reused via submodule), RankingEngine (AND matching & frequency ranking), and REST API Layer (Crow web framework).
- Structured the implementation plan into 5 dedicated markdown modules under docs/DesignProposal/designProposalV1/: introduction.md, indexLoader.md, tokkenizer.md, rankingEnjine.md, and RestApiLayer.md.
- Defined in-memory data structures (HashMap, PostingList, DynamicArray), algorithmic logic for query processing, REST endpoint schemas (/search, /health), and error handling procedures.
Outcome:
- Successfully completed the entire Design Proposal V1 implementation plan in docs/DesignProposal/designProposalV1/.
- Established concrete component contracts and API specifications ready for backend C++ implementation.
