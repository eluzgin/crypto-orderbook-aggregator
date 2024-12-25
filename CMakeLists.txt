cmake_minimum_required(VERSION 3.15)
project(OrderBook)

set(CMAKE_CXX_STANDARD 17)

# Find curl and jsoncpp
find_package(CURL REQUIRED)
find_package(jsoncpp REQUIRED)

# Add your source file
add_executable(OrderBook main.cpp)

# Link libraries
target_include_directories(OrderBook PRIVATE ${CURL_INCLUDE_DIR} ${JsonCpp_INCLUDE_DIRS})
target_link_libraries(OrderBook PRIVATE ${CURL_LIBRARIES} jsoncpp_lib)
