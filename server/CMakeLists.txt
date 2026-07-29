cmake_minimum_required(VERSION 3.14)
project(SearchEngineServer CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Boost for Crow web framework
find_package(Boost REQUIRED COMPONENTS system)

# Include Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/crow
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/ds_library/include
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/ds_library/include/customLibrery
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/Crawler/include
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/sqlite
    ${Boost_INCLUDE_DIRS}
)

# Common Server Sources
set(SERVER_SOURCES
    src/IndexLoader.cpp
    src/RankingEngine.cpp
    src/SearchServer.cpp
    externals/sqlite/sqlite3.c
    externals/Crawler/src/Tokenizer.cpp
)

# Search Engine REST API Executable
add_executable(SearchEngineServer src/main.cpp ${SERVER_SOURCES})
target_compile_definitions(SearchEngineServer PRIVATE CROW_USE_BOOST=1)

if(WIN32)
    target_link_libraries(SearchEngineServer PRIVATE ws2_32 mswsock ${Boost_LIBRARIES})
else()
    target_link_libraries(SearchEngineServer PRIVATE pthread ${Boost_LIBRARIES})
endif()

# Enable Testing and GoogleTest
enable_testing()
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows MSVC runtime override
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Unit Test Executable
add_executable(SearchServerTest tests/SearchServerTest.cpp ${SERVER_SOURCES})
target_compile_definitions(SearchServerTest PRIVATE CROW_USE_BOOST=1)

if(WIN32)
    target_link_libraries(SearchServerTest PRIVATE gtest_main ws2_32 mswsock ${Boost_LIBRARIES})
else()
    target_link_libraries(SearchServerTest PRIVATE gtest_main pthread ${Boost_LIBRARIES})
endif()

include(GoogleTest)
gtest_discover_tests(SearchServerTest)
