project(pdfs)
cmake_minimum_required(VERSION 3.20)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

find_package(Threads REQUIRED)

add_executable(pdfs main.cpp)
target_link_libraries(pdfs ${CURL_LIBRARIES})
target_link_libraries(pdfs Threads::Threads)

# sudo apt-get install libcurl4-openssl-dev