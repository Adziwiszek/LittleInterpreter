#include "../include/Lox.hpp"
#include "../include/Parser.hpp"
#include "../include/Scanner.hpp"
#include "../include/Interpreter.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

void Lox::runFile(std::string path) {
  std::ifstream input(path);
  if(!input.good()) {
    std::cerr << "runFile(path) error\nFailed to open " << path << std::endl;
    return;
  }
  std::stringstream buffer;
  buffer << input.rdbuf();
  run(buffer.str());
  if(hadError) exit(65);
  if(hadRuntimeError) exit(70);
}

void Lox::runPrompt() {
  std::string userInput;
  while(true) {
    std::cout << "> ";
    if(!std::getline(std::cin, userInput)) break;
    run(userInput);
    hadError = false;
  }
}

void Lox::run(std::string source) {
  if(hadError) return;
  Scanner scanner(source, std::make_shared<Lox>(this));
  auto tokens = scanner.scanTokens();
  /*for(const auto& token: tokens) {
    std::cout << token.toString() << "\n";
  }*/

  Parser parser(tokens, std::make_shared<Lox>(this));
  std::vector<StmtPtr> program = parser.parse();
  // Stop if there was a syntax error 
  if(hadError) return;

  Interpreter interpreter(std::make_shared<Lox>(this)); 
  interpreter.interpret(program);
}

void Lox::error(Token token, std::string message) {
  if (token.type == EOF_) {
    report(token.line, "at the end of file", message);
  } else {
    report(token.line, " at '" + token.lexeme + "'", message);
  }
}
void Lox::error(int line, std::string message) {
  report(line, "", message);
}

void Lox::report(int line, std::string where, std::string message) {
  std::cerr << "[line " << line << "] Error " 
    << where << ": " << message << std::endl;
  hadError = true;
}

void Lox::report(Token token, std::string where, std::string message) {
  std::cerr << "[line " << token.line << "] Error " 
    << where << ": " << message << std::endl;
  hadError = true;
}

void Lox::runtimeError(RuntimeError error) {
  std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
  hadRuntimeError = true;
}

