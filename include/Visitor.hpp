#pragma once

#include "Types.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

class Visitor {
public:
  virtual Value visitBinop(Expr::Binop* expr) = 0; 
  virtual Value visitUnop(Expr::Unop* expr) = 0; 
  virtual Value visitGrouping(Expr::Grouping* expr) = 0; 
  virtual Value visitLiteralExpr(Expr::Literal* expr) = 0; 
  virtual Value visitLogical(Expr::Logical* expr) = 0; 

  virtual Value visitExprStmt(Stmt::Expr* exprstmt) = 0; 
  virtual Value visitPrintStmt(Stmt::Print* varstmt) = 0; 
  virtual Value visitBlockStmt(Stmt::Block* stmt) = 0; 
  virtual Value visitIfStmt(Stmt::If* stmt) = 0; 
  virtual Value visitWhileStmt(Stmt::While* stmt) = 0; 
  virtual Value visitBreakStmt(Stmt::Break* stmt) = 0; 

  virtual Value visitVariableExpr(Expr::Variable* var) = 0; 
  virtual Value visitAssign(Expr::Assign* expr) = 0; 
  virtual Value visitVarStmt(Stmt::Var* stmt) = 0; 

  virtual Value visitFunctionStmt(Stmt::Function* stmt) = 0;
  virtual Value visitCall(Expr::Call* expr) = 0; 
};
