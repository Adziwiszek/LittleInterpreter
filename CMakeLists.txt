cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 23)
project(LittleInterpreter)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
  src/main.cpp

  src/Lox.cpp
  src/Scanner.cpp
  src/Parser.cpp
  src/Interpreter.cpp

  src/Environment.cpp
  src/NativeFunctions.cpp
  src/LoxFunction.cpp

  src/Token.cpp
  src/Types.cpp
  src/Expr.cpp
  src/Stmt.cpp
)

set(HEADERS
  include/Lox.hpp
  include/Scanner.hpp
  include/Parser.hpp
  include/Interpreter.hpp

  include/Environment.hpp
  include/Visitor.hpp
  include/NativeFunctions.hpp
  include/LoxFunction.hpp

  include/Token.hpp
  include/Types.hpp
  include/Expr.hpp
  include/Stmt.hpp
)

add_executable(main ${SOURCES} ${HEADERS})

target_link_libraries(main)

