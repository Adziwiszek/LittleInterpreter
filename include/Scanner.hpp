#pragma once

#include <string>
#include <map>

#include "Token.hpp"

class Scanner {
  int start { 0 };
  int current { 0 };
  int line { 1 };
public:
  std::string source;
  Tokens tokens;
  std::map<std::string, TokenType> keywords;

  Scanner(std::string source_);
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

