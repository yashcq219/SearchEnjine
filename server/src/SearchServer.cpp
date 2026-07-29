#include "../include/SearchServer.h"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace searchengine {

// Helper function to trim whitespace from leading and trailing ends
static std::string trimString(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(static_cast<unsigned char>(*start))) {
        start++;
    }
    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(static_cast<unsigned char>(*end)));

    return std::string(start, end + 1);
}

SearchServer::SearchServer(IndexLoader& indexLoader,
                           RankingEngine& rankingEngine,
                           Tokenizer& tokenizer,
                           int port)
    : m_indexLoader(indexLoader),
      m_rankingEngine(rankingEngine),
      m_tokenizer(tokenizer),
      m_port(port),
      m_routesConfigured(false) {
}

SearchServer::~SearchServer() {
}

void SearchServer::configureRoutes() {
    if (m_routesConfigured) {
        return;
    }

    // Endpoint 1: GET /health
    CROW_ROUTE(m_app, "/health")
    ([this](const crow::request& req) {
        return this->handleHealth(req);
    });

    // Endpoint 2: GET /search?q=<query>
    CROW_ROUTE(m_app, "/search")
    ([this](const crow::request& req) {
        return this->handleSearch(req);
    });

    m_routesConfigured = true;
}

crow::response SearchServer::handleHealth(const crow::request& /*req*/) {
    crow::json::wvalue responseJson;
    responseJson["status"] = "Server Running";

    crow::response res(200, responseJson);
    res.set_header("Content-Type", "application/json");
    return res;
}

crow::response SearchServer::handleSearch(const crow::request& req) {
    try {
        // 1. Extract query parameter 'q'
        const char* rawQuery = req.url_params.get("q");
        if (rawQuery == nullptr) {
            crow::json::wvalue errorJson;
            errorJson["error"] = "Missing query parameter 'q'";
            crow::response res(400, errorJson);
            res.set_header("Content-Type", "application/json");
            return res;
        }

        std::string queryStr = trimString(rawQuery);

        // 2. Handle empty or whitespace-only search query
        if (queryStr.empty()) {
            crow::json::wvalue responseJson;
            responseJson["results"] = crow::json::wvalue::list();
            crow::response res(200, responseJson);
            res.set_header("Content-Type", "application/json");
            return res;
        }

        // 3. Tokenize search query using Tokenizer
        DynamicArray<std::string> tokens = m_tokenizer.tokenize(queryStr);
        if (tokens.getSize() == 0) {
            crow::json::wvalue responseJson;
            responseJson["results"] = crow::json::wvalue::list();
            crow::response res(200, responseJson);
            res.set_header("Content-Type", "application/json");
            return res;
        }

        // 4. Delegate ranking to RankingEngine
        const auto& index = m_indexLoader.getIndex();
        DynamicArray<std::string> rankedUrls = m_rankingEngine.rankQuery(tokens, index);

        // 5. Build JSON array of ranked URLs
        crow::json::wvalue::list resultsList;
        for (int i = 0; i < rankedUrls.getSize(); ++i) {
            resultsList.push_back(rankedUrls.get(i));
        }

        crow::json::wvalue responseJson;
        responseJson["results"] = std::move(resultsList);

        crow::response res(200, responseJson);
        res.set_header("Content-Type", "application/json");
        return res;

    } catch (const std::exception& ex) {
        crow::json::wvalue errorJson;
        errorJson["error"] = std::string("Internal Server Error: ") + ex.what();
        crow::response res(500, errorJson);
        res.set_header("Content-Type", "application/json");
        return res;
    } catch (...) {
        crow::json::wvalue errorJson;
        errorJson["error"] = "Internal Server Error";
        crow::response res(500, errorJson);
        res.set_header("Content-Type", "application/json");
        return res;
    }
}

void SearchServer::run() {
    configureRoutes();
    std::cout << "[SearchServer] Starting server on port " << m_port << "..." << std::endl;
    m_app.port(m_port).multithreaded().run();
}

void SearchServer::stop() {
    m_app.stop();
}

int SearchServer::getPort() const {
    return m_port;
}

crow::SimpleApp& SearchServer::getApp() {
    return m_app;
}

} // namespace searchengine
