cmake_minimum_required(VERSION 3.14)
project(P2PChat)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base net)
find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)

include(${wxWidgets_USE_FILE})

add_executable(P2PChat WIN32 main.cpp)

target_include_directories(P2PChat PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(P2PChat 
    PRIVATE 
    ${wxWidgets_LIBRARIES} 
    Boost::system
    nlohmann_json::nlohmann_json
)
