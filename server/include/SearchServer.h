#ifndef SEARCH_SERVER_H
#define SEARCH_SERVER_H

#define CROW_USE_BOOST 1
#include "../externals/crow/crow_all.h"

#include "IndexLoader.h"
#include "RankingEngine.h"
#include "Tokenizer.h"

#include <string>
#include <memory>

namespace searchengine {

/**
 * @class SearchServer
 * @brief REST API Layer server built on the Crow Web Framework.
 * 
 * Thin controller responsible for handling HTTP requests, input validation,
 * delegating search queries to RankingEngine and Tokenizer, and returning
 * formatted JSON responses to client applications.
 */
class SearchServer {
private:
    IndexLoader& m_indexLoader;
    RankingEngine& m_rankingEngine;
    Tokenizer& m_tokenizer;
    int m_port;
    crow::SimpleApp m_app;
    bool m_routesConfigured;

public:
    
    SearchServer(IndexLoader& indexLoader,
                 RankingEngine& rankingEngine,
                 Tokenizer& tokenizer,
                 int port = 8080);

    ~SearchServer();

   
    void configureRoutes();

    
    void run();

    
    void stop();

   
    crow::response handleHealth(const crow::request& req);

    
    crow::response handleSearch(const crow::request& req);

   
    int getPort() const;

    
    crow::SimpleApp& getApp();
};

} 

#endif // SEARCH_SERVER_H
