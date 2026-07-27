# REST API Layer Implementation Plan

---

# Section 1 — Public API

## Overview

The REST API Layer serves as the communication bridge between the frontend and the Search Engine backend. It receives HTTP requests from the client, validates the incoming data, invokes the appropriate backend components, and returns the search results in JSON format.

Unlike the RankingEngine, which is responsible for processing search queries, the REST API Layer contains **no search logic**. Its responsibility is limited to request handling and response generation.

The REST API is implemented using the **Crow Web Framework**, a lightweight C++ web framework designed for building RESTful services with minimal overhead.

---

## Responsibilities

The REST API Layer performs the following operations:

- Start and configure the HTTP server.
- Listen for incoming client requests.
- Parse request parameters.
- Validate user input.
- Invoke the Tokenizer.
- Invoke the RankingEngine.
- Convert search results into JSON.
- Return HTTP responses to the frontend.

---

## Exposed APIs

### 1. Search API

**Endpoint**

```
GET /search?q=<search_query>
```

**Purpose**

Receives the user's search query and returns the ranked search results.

Example

```
GET /search?q=machine learning
```

Response

```json
{
    "results":[
        "url1",
        "url2",
        "url3"
    ]
}
```

---

### 2. Health API

**Endpoint**

```
GET /health
```

**Purpose**

Checks whether the Search Engine server is running correctly.

Example Response

```json
{
    "status":"Server Running"
}
```

---

## Design Decisions

### Decision 1 — Why REST APIs?

The frontend and backend are independent applications.

REST APIs provide a standardized communication mechanism that is:

- Platform independent
- Language independent
- Lightweight
- Easy to test
- Easy to extend

This allows the backend to serve not only the web frontend but also future desktop or mobile applications without modification.

---

### Decision 2 — Why Crow?

The backend is implemented entirely in **C++**, making Crow an appropriate framework because it is specifically designed for high-performance C++ web services.

Crow was selected because it provides:

- Lightweight architecture
- Header-only implementation
- Simple route definition
- Built-in JSON support
- HTTP request parsing
- Multithreaded server support
- High performance with low memory overhead

Compared with larger frameworks, Crow introduces very little complexity while providing all required REST functionality.

---

### Decision 3 — Why Request-Response (Req-Res) Architecture?

The Search Engine follows a **Request-Response communication model**.

The frontend sends a request only when the user performs a search.

```
User Search

↓

HTTP Request

↓

Server Processing

↓

HTTP Response
```

Advantages:

- Simple architecture.
- Stateless communication.
- Easy debugging.
- Low server resource usage.
- Well suited for search applications where each request is independent.

Unlike WebSockets or persistent connections, a search engine does not require continuous bidirectional communication.

---

### Decision 4 — JSON Response Format

Search results are returned as JSON because:

- JavaScript can parse JSON directly.
- Lightweight data representation.
- Human readable.
- Platform independent.
- Widely supported by REST APIs.

---

### Decision 5 — Thin Controller Design

The REST API Layer contains no ranking or searching logic.

Its only responsibility is:

```
Receive Request

↓

Validate Input

↓

Call RankingEngine

↓

Generate JSON

↓

Return Response
```

Keeping the controller lightweight improves maintainability and follows the Single Responsibility Principle.

---

# Section 2 — Internal Representation

## Runtime Workflow

```
User

↓

HTML / CSS / JavaScript

↓

HTTP GET /search

↓

Crow Route

↓

Validate Request

↓

Tokenizer

↓

RankingEngine

↓

Ranked URLs

↓

Convert to JSON

↓

HTTP Response

↓

Frontend
```

---

## Internal Communication

```
Frontend

↓

REST API Layer

↓

Tokenizer

↓

RankingEngine

↓

HashMap
(In-Memory Index)

↓

Search Results

↓

REST API Layer

↓

JSON

↓

Frontend
```

---

## Request-Response Lifecycle

```
Client Request

↓

HTTP Request

↓

Crow Server

↓

Route Matching

↓

API Handler

↓

RankingEngine

↓

JSON Response

↓

HTTP Response
```

Each request is processed independently.

The server stores no information about previous requests.

---

# Section 3 — Failure Handling

| Failure | Handling Strategy |
|----------|-------------------|
| Missing query parameter | Return HTTP 400 (Bad Request). |
| Empty search query | Return an empty JSON array. |
| Invalid endpoint | Return HTTP 404 (Not Found). |
| Unsupported HTTP method | Return HTTP 405 (Method Not Allowed). |
| RankingEngine failure | Return HTTP 500 (Internal Server Error). |
| JSON serialization failure | Return HTTP 500. |
| Server initialization failure | Stop server startup and report error. |

---

# Section 4 — Complexity Analysis

Assume:

- **Q** = Length of the search query.
- **R** = Number of URLs returned.

| Operation | Best | Average | Worst |
|------------|------|----------|--------|
| Route matching | O(1) | O(1) | O(1) |
| Parse request | O(Q) | O(Q) | O(Q) |
| JSON generation | O(R) | O(R) | O(R) |
| HTTP response generation | O(R) | O(R) | O(R) |

The REST API performs minimal computation because all searching and ranking operations are delegated to the RankingEngine.

---

# Section 5 — Future Compatibility

The REST API Layer is intentionally designed as an independent communication layer.

Since all searching logic resides inside the RankingEngine, future improvements to the search algorithm require no modifications to the API layer.

Similarly, changes to the frontend do not affect the backend as long as the API contract remains unchanged.

Future enhancements may include:

- Search suggestions API (`/suggest`)
- Search history API
- Pagination support
- User authentication
- Rate limiting
- Request logging
- Analytics endpoints
- HTTPS support
- API versioning
- Distributed search servers
- Response caching

Because the REST API Layer communicates only through well-defined HTTP interfaces, new endpoints and backend services can be introduced without impacting existing clients.

This modular design ensures long-term maintainability, scalability, and compatibility with future versions of the Search Engine.