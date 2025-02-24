cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 23)
project(LittleInterpreter)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
  src/main.cpp
  src/Token.cpp
)

set(HEADERS
  include/Expr.hpp
  include/Interpreter.hpp
  include/Scanner.hpp
  include/Stmt.hpp
  include/Token.hpp
  include/Types.hpp
  include/Visitor.hpp
  include/Parser.hpp
)

add_executable(main ${SOURCES} ${HEADERS})

target_link_libraries(main)

