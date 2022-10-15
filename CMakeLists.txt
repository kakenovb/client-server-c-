cmake_minimum_required(VERSION 3.24.1)

project(client_server) 

add_compile_options(
    -Werror
    -Wall
)

find_package(Threads REQUIRED)
find_package(Boost 1.54 COMPONENTS log REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

add_executable(server server.cpp)	
add_executable(client client.cpp)	

add_library(wrappers STATIC wrappers.cpp)

target_link_libraries(wrappers ${Boost_LOG_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(server Threads::Threads)
target_link_libraries(server wrappers)
target_link_libraries(server ${Boost_LOG_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(client wrappers)
target_link_libraries(client ${Boost_LOG_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

