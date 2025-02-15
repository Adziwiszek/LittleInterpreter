cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 23)
project(LittleInterpreter)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(main 
    src/main.cpp
)
target_link_libraries(main)

