cmake_minimum_required(VERSION 3.16)
project(CryptoOrderBook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)


# Add included header files
include_directories(include)

# Shared source files as a library
add_library(my_lib src/Order.cpp src/MatchingEngine.cpp src/OrderBook.cpp src/TransactionLogger.cpp src/Utils.cpp)
target_link_libraries(my_lib PRIVATE sqlite3)

# Main executable
add_executable(main.out src/Main.cpp)
target_link_libraries(main.out PRIVATE my_lib)

# Test executable
add_executable(all_tests tests/TestOrder.cpp tests/TestOrderBook.cpp tests/TestMatchingEngine.cpp)
target_link_libraries(all_tests PRIVATE my_lib gtest gtest_main)

enable_testing()
add_test(NAME MyTests COMMAND all_tests)