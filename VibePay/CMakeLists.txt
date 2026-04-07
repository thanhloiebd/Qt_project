cmake_minimum_required(VERSION 3.16)
project(VibePay VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Sql REQUIRED)

add_executable(VibePay
    main.cpp
    MainWindow.cpp
    MainWindow.h
    Database.cpp
    Database.h
    Pages.cpp
    Pages.h
)

target_link_libraries(VibePay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)
