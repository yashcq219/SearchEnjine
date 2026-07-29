#include "../include/IndexLoader.h"
#include "../include/RankingEngine.h"
#include "../include/Tokenizer.h"
#include "../include/SearchServer.h"

#include <iostream>
#include <string>
#include <cstdlib>

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [options]\n"
              << "Options:\n"
              << "  --db <path>     Path to SQLite index database file (default: indexer.db)\n"
              << "  --port <port>   Port for REST API server to listen on (default: 18080)\n"
              << "  --help          Display this help message\n";
}

int main(int argc, char* argv[]) {
    std::string dbPath = "indexer.db";
    int port = 8080;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--db" && i + 1 < argc) {
            dbPath = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::atoi(argv[++i]);
            if (port <= 0 || port > 65535) {
                std::cerr << "Invalid port number specified: " << argv[i] << std::endl;
                return 1;
            }
        } else if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }

    std::cout << "========================================\n";
    std::cout << "       Search Engine REST API Server    \n";
    std::cout << "========================================\n";
    std::cout << "Database Path: " << dbPath << "\n";
    std::cout << "Server Port:   " << port << "\n";

    searchengine::IndexLoader indexLoader;
    searchengine::RankingEngine rankingEngine;
    Tokenizer tokenizer;

    std::cout << "[Initialization] Loading database index from: " << dbPath << "...\n";
    if (!indexLoader.loadFromDatabase(dbPath)) {
        std::cerr << "[Warning] Could not load database from " << dbPath 
                  << ". Server starting with empty in-memory index.\n";
    } else {
        std::cout << "[Initialization] Index loaded successfully (" 
                  << indexLoader.getLoadedWordCount() << " distinct words).\n";
    }

    std::cout << "[Initialization] Starting Search Server...\n";
    searchengine::SearchServer server(indexLoader, rankingEngine, tokenizer, port);
    server.run();

    return 0;
}
