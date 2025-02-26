#include <iostream>
#include <string>
#include <memory>

// Language goals:
// simple types
// one argument functions
// higher order functions
// some i/o
// classes

#include "../include/Types.hpp"
#include "../include/Stmt.hpp"

#include "../include/Lox.hpp"




int main(int argc, char** argv) {
  Lox lox;

  if(argc > 2) {
    std::cout << "Usage: ./dupa [script]" << std::endl;
  } else if(argc == 2) {
    lox.runFile(std::string(argv[1]));
  } else {
    lox.runPrompt();
  }
  return 0;
}

