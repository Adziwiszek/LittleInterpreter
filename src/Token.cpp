#include "../include/Token.hpp"



Token::Token(TokenType type_, std::string lexeme_, int line_, std::optional<Literal> literal_) :
  type {type_}, lexeme {lexeme_}, line {line_}, literal {literal_}
{}

Token::Token(TokenType type_, std::string lexeme_, int line_) :
  Token(type_, lexeme_, line_, std::nullopt) 
{}

std::string Token::toString() const {
  std::string literalStr = literal.has_value() ? literal->toString() : "";
  return tokenTypeToString(type) + " " + lexeme + " " + literalStr;
}

std::string tokenTypeToString(TokenType type) {
  switch(type) {
    case LEFT_PAREN: return "LEFT_PAREN"; break;
    case RIGHT_PAREN: return "RIGHT_PAREN"; break;
    case LEFT_BRACE: return "LEFT_BRACE"; break;
    case RIGHT_BRACE: return "RIGHT_BRACE"; break;
    case COMMA: return "COMMA"; break;
    case DOT: return "DOT"; break;
    case MINUS: return "MINUS"; break;
    case PLUS: return "PLUS"; break;
    case SEMICOLON: return "SEMICOLON"; break;
    case SLASH: return "SLASH"; break;
    case STAR: return "STAR"; break;
    case BANG: return "BANG"; break;
    case BANG_EQUAL: return "BANG_EQUAL"; break;
    case EQUAL: return "EQUAL"; break;
    case EQUAL_EQUAL: return "EQUAL_EQUAL"; break;
    case GREATER: return "GREATER"; break;
    case GREATER_EQUAL: return "GREATER_EQUAL"; break;
    case LESS: return "LESS"; break;
    case LESS_EQUAL: return "LESS_EQUAL"; break;
    case IDENTIFIER: return "IDENTIFIER"; break;
    case STRING: return "STRING"; break;
    case NUMBER: return "NUMBER"; break;
    case AND: return "AND"; break;
    case CLASS: return "CLASS"; break;
    case ELSE: return "ELSE"; break;
    case FALSE: return "FALSE"; break;
    case FUN: return "FUN"; break;
    case FOR: return "FOR"; break;
    case IF: return "IF"; break;
    case NIL: return "NIL"; break;
    case OR: return "OR"; break;
    case PRINT: return "PRINT"; break;
    case RETURN: return "RETURN"; break;
    case SUPER: return "SUPER"; break;
    case THIS: return "THIS"; break;
    case TRUE: return "TRUE"; break;
    case VAR: return "VAR"; break;
    case BREAK: return "BREAK"; break;
    case WHILE: return "WHILE"; break;
    case EOF_: return "EOF_"; break;
    case PLUSPLUS: return "++"; break;
    case MINUSMINUS: return "--"; break;
  }
  return "unknow token type";
}
