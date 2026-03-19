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

# add_executable(program
    #src/Main.cpp
    #src/OrderBook.cpp
    #src/Order.cpp
    #src/TransactionLogger.cpp
    #src/MatchingEngine.cpp
    #)

#target_link_libraries(program sqlite3)

add_executable(my_tests tests/TestOrder.cpp)
target_link_libraries(my_tests PRIVATE gtest gtest_main)

# Register tests with ctest
enable_testing()
add_test(NAME MyTests COMMAND my_tests)