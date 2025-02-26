#pragma once

#include <memory>
#include <vector>
#include <iostream>

#include "Visitor.hpp"
#include "Lox.hpp"
#include "Types.hpp"
#include "Environment.hpp"
#include "NativeFunctions.hpp"

// Forward declarations -------------------------------------------------------
class Value;
class Token;
class Environment;

namespace Stmt {
    class Stmt;
    using StmtPtr = std::shared_ptr<Stmt>;
    using Stmts = std::vector<StmtPtr>;
}

namespace Expr {
    class Expr;
    using ExprPtr = std::shared_ptr<Expr>;
    using Exprs = std::vector<ExprPtr>;
}

using Stmt::Stmts, Stmt::StmtPtr, Expr::Exprs, Expr::ExprPtr;
// ----------------------------------------------------------------------------

class BreakLoop : public std::runtime_error {
  public:
    BreakLoop() : std::runtime_error("") {}
    ~BreakLoop() = default;
};

class Interpreter : public Visitor {
  std::shared_ptr<Environment> environment;
  std::shared_ptr<Lox> lox;

  void checkNumberOperand(Token op, Value& val) const; 
  void checkNumberOperands(Token op, Value& left, Value& right) const; 
  void reportDifferentTypesOperands() const; 
  bool isTruthy(Value val) const; 
  bool isEqual(Value left, Value right) const; 
public:
  std::shared_ptr<Environment> globals;

  virtual Value visitBinop(Expr::Binop* expr) override; 
  virtual Value visitUnop(Expr::Unop* expr) override; 
  virtual Value visitGrouping(Expr::Grouping* expr) override; 
  virtual Value visitLiteralExpr(Expr::Literal* expr) override; 

  virtual Value visitExprStmt(Stmt::Expr* exprstmt) override; 
  virtual Value visitPrintStmt(Stmt::Print* varstmt) override; 
  virtual Value visitVarStmt(Stmt::Var* stmt) override; 

  virtual Value visitVariableExpr(Expr::Variable* var) override; 
  virtual Value visitAssign(Expr::Assign* expr) override; 
  virtual Value visitBlockStmt(Stmt::Block* stmt) override; 
  virtual Value visitIfStmt(Stmt::If* stmt) override; 
  virtual Value visitLogical(Expr::Logical* expr) override; 
  virtual Value visitWhileStmt(Stmt::While* stmt) override; 
  virtual Value visitBreakStmt(Stmt::Break* stmt) override; 

  virtual Value visitCall(Expr::Call* expr) override; 
  virtual Value visitFunctionStmt(Stmt::Function* stmt) override;

  Interpreter(std::shared_ptr<Lox> lox);

  void execute(const StmtPtr& stmt); 
  void executeBlock(const Stmts& statements,
      std::shared_ptr<Environment> env); 
  Value evaluate(Expr::Expr* expr); 

  void interpret(const Stmts& program); 
};
