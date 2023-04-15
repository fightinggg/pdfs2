project(pdfs)
cmake_minimum_required(VERSION 3.20)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

find_package(Threads REQUIRED)

include_directories(/usr/include/tirpc/)

add_executable(pdfs main.cpp)
target_link_libraries(pdfs ${CURL_LIBRARIES})
target_link_libraries(pdfs Threads::Threads)


#add_executable(nfsserver oncrpc/nfs_svc.c oncrpc/nfs_server.c  oncrpc/nfs_xdr.c oncrpc/nfs_clnt.c)
#target_link_libraries(nfsserver tirpc)
#
#add_executable(nfsclient oncrpc/nfs_client.c  oncrpc/nfs_xdr.c oncrpc/nfs_clnt.c)
#target_link_libraries(nfsclient tirpc)
# sudo apt-get install libcurl4-openssl-dev