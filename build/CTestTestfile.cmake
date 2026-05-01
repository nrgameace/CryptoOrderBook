# CMake generated Testfile for 
# Source directory: /home/nickregas/Desktop/CryptoProject/CryptoOrderBook
# Build directory: /home/nickregas/Desktop/CryptoProject/CryptoOrderBook/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(MyTests "/home/nickregas/Desktop/CryptoProject/CryptoOrderBook/build/all_tests")
set_tests_properties(MyTests PROPERTIES  _BACKTRACE_TRIPLES "/home/nickregas/Desktop/CryptoProject/CryptoOrderBook/CMakeLists.txt;51;add_test;/home/nickregas/Desktop/CryptoProject/CryptoOrderBook/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
subdirs("_deps/httplib-build")
subdirs("_deps/nlohmann_json-build")
