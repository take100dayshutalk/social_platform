cmake_minimum_required(VERSION 3.5)
project(mainobject VERSION 1.0 LANGUAGES CXX)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets HttpServer Sql)
add_subdirectory(server)
add_subdirectory(databasepool)
add_subdirectory(mainwidget)
add_subdirectory(SmtpMime)

add_executable(mainobject
    main.cpp
)
target_link_libraries(mainobject PRIVATE Qt6::Widgets Qt6::HttpServer Qt6::Sql server databasepool mainwidget SmtpMime)
