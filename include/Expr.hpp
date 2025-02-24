#pragma once

#include <memory>
#include <vector>

#include "Types.hpp"
#include "Token.hpp"

class Visitor;

namespace Expr {

class Expr {
public:
  virtual Value accept(Visitor* visitor) = 0;
  virtual ~Expr() = default;
};

using ExprPtr = std::shared_ptr<Expr>;
using Exprs  = std::vector<ExprPtr>;

class Call : public Expr {
public:
  ExprPtr callee;
  Token paren;
  Exprs arguments;
  Call(ExprPtr callee, Token paren, 
       const std::vector<ExprPtr>& arguments);
  virtual Value accept(Visitor* visitor) override; 
};

class Logical : public Expr {
public:
  ExprPtr left;
  Token op;
  ExprPtr right;

  Logical(ExprPtr left, Token op, ExprPtr right);
  virtual Value accept(Visitor* visitor) override; 
};

class Assign : public Expr {
public:
  Token name;
  ExprPtr value;

  Assign(Token name, ExprPtr expr);
  virtual Value accept(Visitor* visitor) override; 
};

class Variable : public Expr {
public:
  Token name;
  Variable(Token name);
  virtual Value accept(Visitor* visitor) override; 
};

class Binop : public Expr {
public:
  ExprPtr left;
  ExprPtr right;
  Token op;

  Binop(ExprPtr left, Token op, ExprPtr right);
  virtual Value accept(Visitor* visitor) override; 
};

class Unop : public Expr {
public:
  ExprPtr expr;
  Token op;

  Unop(Token op, ExprPtr expr);
  virtual Value accept(Visitor* visitor) override; 
};

class Grouping : public Expr {
public:
  ExprPtr expr;

  Grouping(ExprPtr expr) : expr { std::move(expr) } {}
  virtual Value accept(Visitor* visitor) override; 
};

class Literal : public Expr {
public:
  LiteralPtr value;
  Literal(LiteralPtr value);
  Literal(bool b);
  Literal(float f);
  Literal(std::string s);
  Literal(Nil s);
  virtual Value accept(Visitor* visitor) override; 
};

};

