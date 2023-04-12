project(pdfs)
cmake_minimum_required(VERSION 3.20)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

find_package(Threads REQUIRED)

include_directories(/usr/include/tirpc/)

add_executable(pdfs main.cpp)
target_link_libraries(pdfs ${CURL_LIBRARIES})
target_link_libraries(pdfs Threads::Threads)


add_executable(nfsserver oncrpc/nfs_svc.c oncrpc/nfs_server.c)
target_link_libraries(nfsserver ${CURL_LIBRARIES})
target_link_libraries(nfsserver Threads::Threads)
# sudo apt-get install libcurl4-openssl-dev