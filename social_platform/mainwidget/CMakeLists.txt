cmake_minimum_required(VERSION 3.5)
project(mainwidget VERSION 1.0 LANGUAGES CXX)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(mainwidget STATIC
    mainwidget.h
    mainwidget.cpp

)

target_include_directories(mainwidget PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mainwidget PRIVATE Qt6::Widgets)

