cmake_minimum_required(VERSION 3.16)
project(Lab_2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(Lab_2
    main.cpp
    MainWindow.cpp
    MainWindow.h
    Reference.cpp
    Reference.h
    Context.cpp
    Context.h
    DataManager.cpp
    DataManager.h
    ReferenceDialog.cpp
    ReferenceDialog.h
    ContextDialog.cpp
    ContextDialog.h
    ExportDialog.cpp
    ExportDialog.h
)

target_link_libraries(Lab_2 Qt6::Core Qt6::Widgets)
