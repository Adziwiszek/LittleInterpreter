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

struct This : public Expr {
  Token keyword;
  This(Token keyword);
  virtual Value accept(Visitor* visitor) override; 
};

class Call : public Expr {
public:
  ExprPtr callee;
  Token paren;
  Exprs arguments;
  Call(ExprPtr callee, Token paren, 
       const std::vector<ExprPtr>& arguments);
  virtual Value accept(Visitor* visitor) override; 
};

class Get : public Expr {
public:
  ExprPtr object;
  Token name;

  Get(ExprPtr object, Token name);
  virtual Value accept(Visitor* visitor) override; 
};

class Set : public Expr {
public:
  ExprPtr object;
  Token name;
  ExprPtr value;

  Set(ExprPtr object, Token name, ExprPtr value);
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
  Assign(const Assign& other);
  virtual Value accept(Visitor* visitor) override; 
};

class Variable : public Expr {
public:
  Token name;
  Variable(Token name);
  Variable(const Variable& other);
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

  Grouping(ExprPtr expr);
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

