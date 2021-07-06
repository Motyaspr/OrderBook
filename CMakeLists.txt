cmake_minimum_required(VERSION 3.19)
project(OrderBook)

set(CMAKE_CXX_STANDARD 20)

include_directories(common)
include_directories(Map_vector_order_book)
include_directories(HT_list_order_book)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG master)


FetchContent_MakeAvailable(googletest googlebenchmark)

enable_testing()

add_executable(my_test tests/test.cc HT_list_order_book/OrderBookHT.cpp HT_list_order_book/OrderBookHT.h Map_vector_order_book/OrderBookVector.h)
add_executable(my_bench benchmarks/my_benchmark.cc HT_list_order_book/OrderBookHT.cpp HT_list_order_book/OrderBookHT.h Map_vector_order_book/OrderBookVector.h)

target_link_libraries(my_test gtest_main)
target_link_libraries(my_bench benchmark::benchmark)

include(GoogleTest)

gtest_discover_tests(my_test)

add_executable(OrderBookHT main.cpp common/Element.h HT_list_order_book/OrderBookHT.cpp HT_list_order_book/OrderBookHT.h)

add_executable(OrderBookVector main2.cpp common/Element.h Map_vector_order_book/OrderBookVector.h)