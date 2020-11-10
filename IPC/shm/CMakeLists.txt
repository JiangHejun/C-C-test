cmake_minimum_required(VERSION 2.8)

project(IPC_shm_sem)
message("Project Name: " ${PROJECT_NAME})

set(CMAKE_C_COMPILE "gcc")
set(CMAKE_CXX_COMPILE "g++")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

link_directories(/usr/local/lib)

add_executable(server server.cpp)
add_executable(client client.cpp)