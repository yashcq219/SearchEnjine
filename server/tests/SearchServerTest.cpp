#include <gtest/gtest.h>
#include "../include/SearchServer.h"
#include "../include/IndexLoader.h"
#include "../include/RankingEngine.h"
#include "../include/Tokenizer.h"

using namespace searchengine;

class SearchServerTest : public ::testing::Test {
protected:
    IndexLoader indexLoader;
    RankingEngine rankingEngine;
    Tokenizer tokenizer;
    std::unique_ptr<SearchServer> server;

    void SetUp() override {
        server = std::make_unique<SearchServer>(indexLoader, rankingEngine, tokenizer, 18080);
    }
};

TEST_F(SearchServerTest, HealthEndpointReturnsServerRunning) {
    crow::request req;
    req.url = "/health";

    crow::response res = server->handleHealth(req);
    EXPECT_EQ(res.code, 200);

    auto json = crow::json::load(res.body);
    ASSERT_TRUE(static_cast<bool>(json));
    EXPECT_EQ(json["status"].s(), "Server Running");
}

TEST_F(SearchServerTest, SearchEndpointMissingQueryParameterReturns400) {
    crow::request req;
    req.url = "/search";

    crow::response res = server->handleSearch(req);
    EXPECT_EQ(res.code, 400);

    auto json = crow::json::load(res.body);
    ASSERT_TRUE(static_cast<bool>(json));
    EXPECT_EQ(json["error"].s(), "Missing query parameter 'q'");
}

TEST_F(SearchServerTest, SearchEndpointEmptyQueryReturnsEmptyResults) {
    crow::request req;
    req.url = "/search?q=";
    req.url_params = crow::query_string("?q=");

    crow::response res = server->handleSearch(req);
    EXPECT_EQ(res.code, 200);

    auto json = crow::json::load(res.body);
    ASSERT_TRUE(static_cast<bool>(json));
    ASSERT_TRUE(json.has("results"));
    EXPECT_EQ(json["results"].size(), 0u);
}

TEST_F(SearchServerTest, SearchEndpointValidQueryReturnsResponse) {
    crow::request req;
    req.url = "/search?q=machine";
    req.url_params = crow::query_string("?q=machine");

    crow::response res = server->handleSearch(req);
    EXPECT_EQ(res.code, 200);

    auto json = crow::json::load(res.body);
    ASSERT_TRUE(static_cast<bool>(json));
    ASSERT_TRUE(json.has("results"));
}
