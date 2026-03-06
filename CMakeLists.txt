cmake_minimum_required(VERSION 3.16)
project(CryptoOrderBook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Add included header files
include_directories(include)

add_executable(program
    src/Main.cpp
    src/OrderBook.cpp
    src/Order.cpp
    src/TransactionLogger.cpp
    src/MatchingEngine.cpp
    )

target_link_libraries(program sqlite3)