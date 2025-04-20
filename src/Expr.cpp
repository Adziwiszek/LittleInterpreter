#include "../include/Expr.hpp"
#include "../include/Visitor.hpp"

#include <iostream>

// this is Literal from types, used for representing literals in language
// Expr::Literal is an expression for a literal
using LiteralType = Literal;

Expr::Literal::Literal(LiteralPtr value) 
  : value { std::move(value) } {}
Expr::Literal::Literal(bool b) 
  : value { std::make_shared<LiteralType>(b) } {}
Expr::Literal::Literal(float f) 
  : value { std::make_shared<LiteralType>(f) } {}
Expr::Literal::Literal(std::string s) 
  : value { std::make_shared<LiteralType>(s) } {}
Expr::Literal::Literal(Nil s) 
  : value { std::make_shared<LiteralType>(s) } {}

Value Expr::Literal::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitLiteralExpr(this);
}

using namespace Expr;

This::This(Token keyword) : keyword{keyword} {}
Value This::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitThisExpr(this);
}

Call::Call(std::shared_ptr<Expr> callee, Token paren,
    const std::vector<std::shared_ptr<Expr>>& arguments)
  : callee{callee}, paren{paren}, arguments{arguments} {}
Value Call::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitCall(this);
}

Get::Get(ExprPtr object, Token name) : object{object}, name{name} {}

Value Get::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitGetExpr(this);
}

Set::Set(ExprPtr object, Token name, ExprPtr value) 
  : object{object}, name{name}, value{value} {}

Value Set::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitSetExpr(this);
}

Logical::Logical(std::shared_ptr<Expr> left,
    Token op,
    std::shared_ptr<Expr> right)
  : left { std::move(left) }, op { op }, right { std::move(right) }
  {}
Value Logical::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitLogical(this);
}

Assign::Assign(Token name, std::shared_ptr<Expr> expr)
  : name { name }, value { std::move(expr) } { }

Assign::Assign(const Assign& other)
  : name { other.name }, value { std::move(other.value) } {}

Value Assign::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitAssign(this);
}

Variable::Variable(Token name) : name { name } {}
Variable::Variable(const Variable& other) : name { other.name } {}
Value Variable::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitVariableExpr(this);
}

Binop::Binop(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
  : left { std::move(left) }, right { std::move(right) }, op { op }
  { }
Value Binop::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitBinop(this);
}

Unop::Unop(Token op, std::shared_ptr<Expr> expr)
  : expr { std::move(expr) }, op { op }
{ }
Value Unop::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitUnop(this);
}

Grouping::Grouping(std::shared_ptr<Expr> expr) : expr { std::move(expr) } {}
Value Grouping::accept(Visitor* visitor) {
  if(!visitor) return Nil();
  return visitor->visitGrouping(this);
}

