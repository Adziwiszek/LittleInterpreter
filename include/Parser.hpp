#pragma once

#include <memory>
#include <vector>
#include <iostream>

#include "Lox.hpp"
#include "Stmt.hpp"
#include "Expr.hpp"

// Forward declarations
class Value;
class Token;
class Environment;
class Callable;

/*namespace Stmt {
    class Stmt;
    using StmtPtr = std::shared_ptr<Stmt>;
    using Stmts = std::vector<StmtPtr>;
}*/

/*namespace Expr {
    class Expr;
    using ExprPtr = std::shared_ptr<Expr>;
    using Exprs = std::vector<ExprPtr>;
}*/

using Stmt::Stmts, Stmt::StmtPtr, Expr::Exprs, Expr::ExprPtr;

class ParseError : public std::runtime_error {
public:
  ParseError(const std::string& message) : std::runtime_error(message) {}
  ~ParseError() = default;
};

class Parser {
  std::shared_ptr<Lox> lox;
  Tokens tokens;
  int current;

  ExprPtr expression(); 
  ExprPtr orExpr(); 
  ExprPtr andExpr(); 
  ExprPtr assignment(); 
  ExprPtr equality(); 
  ExprPtr comparison(); 
  ExprPtr term(); 
  ExprPtr factor(); 
  ExprPtr unary(); 
  ExprPtr call(); 
  ExprPtr finishCall(ExprPtr callee); 
  ExprPtr primary(); 

  template <typename... TokenTypes>
  bool match(TokenTypes... types); 
  Token consume(TokenType type, std::string message); 
  bool check(TokenType type); 
  Token advance(); 
  bool isAtEnd(); 
  Token peek(); 
  Token previous(); 
  void synchronize(); 

  ParseError parserError(Token token, std::string message); 

  StmtPtr statement(); 
  StmtPtr classDeclaration();
  StmtPtr returnStatement();
  StmtPtr printStatement(); 
  StmtPtr expressionStatement(); 
  StmtPtr whileStatement(); 
  StmtPtr forStatement(); 
  StmtPtr breakStatement(); 
  StmtPtr ifStatement(); 
  Stmts block(); 
  StmtPtr declaration(); 
  std::shared_ptr<Stmt::Function> function(std::string kind); 
  StmtPtr varDeclaration(); 
public:
  Parser(std::vector<Token> tokens, std::shared_ptr<Lox> lox);
  Stmts parse(); 
};

