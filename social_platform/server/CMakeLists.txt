cmake_minimum_required(VERSION 3.5)
project(server VERSION 1.0 LANGUAGES CXX)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS HttpServer Sql)

add_library(server STATIC
    server.h
    server.cpp
    resultcode.h
    analysisdata.h
    analysisdata.cpp
    encipher.h
    encipher.cpp
)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(server PRIVATE Qt6::HttpServer Qt6::Core Qt6::Sql)
