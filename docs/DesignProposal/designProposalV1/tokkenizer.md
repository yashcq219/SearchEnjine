# Tokenizer Implementation Plan

---

# Section 1 — Public API

## Overview

The Tokenizer is responsible for converting the user's search query into a sequence of normalized searchable words. Instead of implementing a new Tokenizer inside the Search Engine, the existing implementation developed during the Indexer project is reused through a Git submodule.

Reusing the same Tokenizer guarantees that the exact preprocessing rules applied while indexing documents are also applied when processing user queries. As a result, the vocabulary generated during searching is always consistent with the vocabulary stored in the inverted index.

The Search Engine treats the Tokenizer as an external reusable library and simply invokes its functionality whenever a search request is received.

---

## Responsibilities

The Tokenizer performs the following operations:

- Receive the raw search query from the REST API.
- Separate the query into individual words.
- Remove punctuation symbols.
- Remove common stop words.
- Return the remaining searchable words as a DynamicArray.
- Forward the processed tokens to the RankingEngine.

---

## Input

The Tokenizer receives the complete search query entered by the user.

Example:

```
C++ Search Engine and Crow Framework
```

---

## Output

```
DynamicArray<String>

↓

cpp

search

engine

crow

framework
```

Only meaningful searchable words are returned.

---

## Design Decisions

### Decision 1 — Reuse Existing Tokenizer

The Search Engine reuses the Tokenizer developed during the Indexer project through a Git submodule.

#### Reason

The same preprocessing rules must be applied during both indexing and searching.

If different tokenization rules were used, the generated query words might not match the indexed words, leading to incorrect or missing search results.

Using a shared implementation guarantees consistency across both projects while avoiding duplicate code.

---

### Decision 2 — Git Submodule Integration

Instead of copying the Tokenizer source files into the Search Engine repository, the entire Indexer library is imported as a Git submodule.

#### Reason

Using a Git submodule provides several advantages:

- Eliminates duplicate source code.
- Allows bug fixes in one project to automatically benefit the other.
- Simplifies maintenance.
- Encourages modular software architecture.
- Keeps both projects synchronized.

---

### Decision 3 — Remove Stop Words

Common English words such as

```
the

is

and

of

to
```

appear in almost every document.

Searching for these words contributes very little to result quality while increasing unnecessary lookups.

Removing stop words:

- reduces the number of query terms,
- decreases HashMap lookups,
- improves ranking performance.

---

### Decision 4 — Remove Punctuation

Characters such as

```
.

,

!

?

()

[]

{}
```

do not contribute to search semantics.

They are treated as delimiters between words.

This allows queries like

```
C++, Java!
```

to be converted into

```
cpp

java
```

---

### Decision 5 — Preserve Word Order

The Tokenizer preserves the order in which searchable words appear in the query.

Example

```
machine learning search
```

becomes

```
machine

learning

search
```

Maintaining the original order allows future implementations to support phrase searching and proximity-based ranking.

---

# Section 2 — Internal Representation

The Tokenizer maintains an internal stop-word dictionary and produces a DynamicArray of searchable words.

```
                   Tokenizer

                       │

      ┌────────────────┴────────────────┐

      ▼                                 ▼

 Stop Word Dictionary          DynamicArray<String>

      │                                 │

the                              machine

is                               learning

and                              search

to

for
```

---

## Query Processing Workflow

```
User Query

↓

Tokenizer

↓

Split into Words

↓

Remove Punctuation

↓

Convert to Lowercase

↓

Remove Stop Words

↓

DynamicArray<String>

↓

RankingEngine
```

---

## Why DynamicArray?

The output of the Tokenizer is stored inside a DynamicArray because:

- query words are processed sequentially,
- the number of query words is usually small,
- insertion occurs only during tokenization,
- traversal is extremely fast due to contiguous memory.

The RankingEngine simply iterates over the resulting DynamicArray without requiring random insertions or deletions.

---

# Section 3 — Failure Handling

| Failure | Handling Strategy |
|----------|-------------------|
| Empty query | Return an empty DynamicArray. |
| Query containing only punctuation | Return an empty DynamicArray. |
| Query containing only stop words | Return an empty DynamicArray. |
| Consecutive delimiters | Ignore extra delimiters. |
| Mixed uppercase and lowercase letters | Normalize all words before stop-word comparison. |
| Unsupported symbols | Treat unsupported symbols as delimiters and continue processing. |

The Tokenizer never terminates the search process because of malformed input. Invalid characters are simply ignored while valid words continue to be processed.

---

# Section 4 — Complexity Analysis

Assume:

- **N** = Length of the input query.
- **W** = Number of extracted words.

| Operation | Best | Average | Worst |
|------------|------|----------|--------|
| Scan query characters | O(N) | O(N) | O(N) |
| Stop-word lookup | O(1) | O(1) | O(k) |
| Append token | O(1) | O(1) | O(W) |
| Complete tokenization | O(N) | O(N) | O(N) |

Since every character is examined exactly once, the Tokenizer operates in linear time with respect to the length of the query.

---

# Section 5 — Future Compatibility

The Tokenizer is shared between the Indexer and the Search Engine through a Git submodule, making it a reusable library rather than a project-specific implementation.

This design ensures that any improvements made to the Tokenizer automatically become available to both projects without requiring duplicate modifications.

The current implementation performs:

- punctuation removal,
- lowercase normalization,
- stop-word removal.

Future versions can be extended to support additional preprocessing techniques without affecting the RankingEngine or REST API.

Possible future enhancements include:

- Stemming
- Lemmatization
- Unicode normalization
- Multilingual tokenization
- Synonym expansion
- Spell correction
- Emoji and special character handling

Because the RankingEngine only consumes the DynamicArray produced by the Tokenizer, these improvements can be implemented internally while preserving the existing interface.

The Git submodule architecture therefore promotes code reuse, consistency between indexing and searching, and easier long-term maintenance of the overall Search Engine project.