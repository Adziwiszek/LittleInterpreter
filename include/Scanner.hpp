#pragma once

#include <string>
#include <map>

#include "Token.hpp"
#include "Lox.hpp"

class Scanner {
  Lox& lox;

  int start { 0 };
  int current { 0 };
  int line { 1 };
public:
  std::string source;
  Tokens tokens;
  std::map<std::string, TokenType> keywords;

  Scanner(std::string source, Lox& lox);
  Tokens scanTokens(); 
  void scanToken(); 

  void identifierLex(); 
  void numberLex(); 
  void stringLex(); 

  char peekNext(); 
  char peek(); 
  bool match(char expected); 
  char advance(); 
  bool isAtEnd(); 

  bool isDigit(char c); 
  bool isAlphaNumeric(char c); 

  void addToken(TokenType type); 
  void addToken(TokenType type, std::optional<Literal> literal); 

  void setupKeywords();
};

