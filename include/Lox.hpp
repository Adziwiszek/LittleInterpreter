#pragma once

#include "Token.hpp"
#include <string>
#include <stdexcept>

class RuntimeError : public std::runtime_error {
  public:
    Token token;
    RuntimeError(Token token, const std::string& message) : 
      token { token }, std::runtime_error(message) {}
    ~RuntimeError() = default;
};

class Lox {
public:
  bool hadError { false };
  bool hadRuntimeError { false };

  void runFile(std::string path);
  void runPrompt();
  void run(std::string source);
  void error(Token token, std::string message);
  void error(int line, std::string message);
  void report(Token token, std::string where, std::string message);
  void report(int line, std::string where, std::string message);
  void runtimeError(RuntimeError error);
};
