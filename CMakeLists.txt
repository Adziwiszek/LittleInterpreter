cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)
project(LittleInterpreter)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Automatically find all source and header files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp)

add_executable(main ${SOURCES} ${HEADERS})

target_link_libraries(main)
