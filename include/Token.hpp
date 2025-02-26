#pragma once

#include <string>
#include <optional>
#include <vector>

#include "Types.hpp"

enum TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,
  PLUSPLUS, MINUSMINUS,

  // Literals.
  IDENTIFIER, STRING, NUMBER,

  // Keywords.
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
  BREAK,

  EOF_
};
std::string tokenTypeToString(TokenType type);

class Token {
public:
  TokenType type;
  std::string lexeme;
  std::optional<Literal> literal;
  int line;

  Token(TokenType type, std::string lexeme, int line,
      std::optional<Literal> literal);
  Token(TokenType type, std::string lexeme, int line);
  std::string toString() const; 
};

using Tokens = std::vector<Token>;
