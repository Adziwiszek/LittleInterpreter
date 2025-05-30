#include "../include/Stmt.hpp"
#include "../include/Visitor.hpp"

Stmt::Expr::Expr(ExprPtr expr) : expr {std::move(expr)} {}
Value Stmt::Expr::accept(Visitor<Value>* visitor) {
  if(!visitor) return Nil();
  return visitor->visitExprStmt(this);
}

Type Stmt::Expr::accept(Visitor<Type>* visitor) {
  if(!visitor) return Type::NIL;
  return visitor->visitExprStmt(this);
}

using namespace Stmt;

Class::Class(Token name, const Methods& methods) : name{name}, methods{methods} {}

Value Class::accept(Visitor<Value>* visitor) {
  if(!visitor) return Nil();
  return visitor->visitClassStmt(this);
}

Type Class::accept(Visitor<Type>* visitor) {
  if(!visitor) return Type::NIL;
  return visitor->visitClassStmt(this);
}


Return::Return(const Token& keyword, const ExprPtr& value)
  : keyword { keyword }, value { std::move(value) } {}

Value Return::accept(Visitor<Value>* visitor) {
  if(!visitor) return Nil();
  return visitor->visitReturnStmt(this);
}

Type Return::accept(Visitor<Type>* visitor) {
  if(!visitor) return Type::NIL;
  return visitor->visitReturnStmt(this);
}

Function::Function(Token name, const std::vector<Token>& args,
    const std::vector<StmtPtr>& body)
  : name{name}, body{body}, args{args}
{}

Function::Function(const Function& other) 
  : name {other.name}, body {other.body}, args{other.args}
{}

Value Function::accept(Visitor<Value>* visitor) {
  if(!visitor) return Nil();
  return visitor->visitFunctionStmt(this);
}

Type Function::accept(Visitor<Type>* visitor) {
  if(!visitor) return Type::NIL;
  return visitor->visitFunctionStmt(this);
}

Break::Break(Token keyword) : keyword{keyword} {}
Value Break::accept(Visitor<Value>* visitor) {
  if(!visitor) return Nil();
  return visitor->visitBreakStmt(this);
}

Type Break::accept(Visitor<Type>* visitor) {
  if(!visitor) return Type::NIL;
  return visitor->visitBreakStmt(this);
}

While::While(ExprPtr condition, StmtPtr body)
  : condition{ std::move(condition) }, body{ std::move(body) } {}
Value While::accept(Visitor<Value>* visitor) {
  if(!visitor) return Nil();
  return visitor->visitWhileStmt(this);
}

Type While::accept(Visitor<Type>* visitor) {
  if(!visitor) return Type::NIL;
  return visitor->visitWhileStmt(this);
}

If::If(ExprPtr& condition, 
    StmtPtr& thenBranch,
    StmtPtr& elseBranch)
  : condition(std::move(condition)),
  thenBranch(std::move(thenBranch)),
  elseBranch(std::move(elseBranch)) {}

Value If::accept(Visitor<Value>* visitor) {
  return visitor->visitIfStmt(this);
}


Type If::accept(Visitor<Type>* visitor) {
  return visitor->visitIfStmt(this);
}

Block::Block() : statements {} {}
Block::Block(const std::vector<StmtPtr>& otherStatements)
  : statements(otherStatements) {}
Block::Block(const Block& other) { Block(other.statements); }


Value Block::accept(Visitor<Value>* visitor) {
  if(!visitor) return Nil();
  return visitor->visitBlockStmt(this);
}

Type Block::accept(Visitor<Type>* visitor) {
  if(!visitor) return Type::NIL;
  return visitor->visitBlockStmt(this);
}


Print::Print(ExprPtr expr) : expr {std::move(expr)} {}
Value Print::accept(Visitor<Value>* visitor) {
  if(!visitor) return Nil();
  return visitor->visitPrintStmt(this);
}

Type Print::accept(Visitor<Type>* visitor) {
  if(!visitor) return Type::NIL;
  return visitor->visitPrintStmt(this);
}

Var::Var(ExprPtr expr, Token name) :
  initializer {std::move(expr)}, name { name } {}
Value Var::accept(Visitor<Value>* visitor) {
  if(!visitor) return Nil();
  return visitor->visitVarStmt(this);
}

Type Var::accept(Visitor<Type>* visitor) {
  if(!visitor) return Type::NIL;
  return visitor->visitVarStmt(this);
}

