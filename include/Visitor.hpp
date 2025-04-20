#pragma once

#include "Types.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

template <typename T>
class Visitor {
public:
  virtual T visitBinop(Expr::Binop* expr) = 0; 
  virtual T visitUnop(Expr::Unop* expr) = 0; 
  virtual T visitGrouping(Expr::Grouping* expr) = 0; 
  virtual T visitLiteralExpr(Expr::Literal* expr) = 0; 
  virtual T visitLogical(Expr::Logical* expr) = 0; 

  virtual T visitExprStmt(Stmt::Expr* exprstmt) = 0; 
  virtual T visitPrintStmt(Stmt::Print* varstmt) = 0; 
  virtual T visitBlockStmt(Stmt::Block* stmt) = 0; 
  virtual T visitIfStmt(Stmt::If* stmt) = 0; 
  virtual T visitWhileStmt(Stmt::While* stmt) = 0; 
  virtual T visitBreakStmt(Stmt::Break* stmt) = 0; 

  virtual T visitVariableExpr(Expr::Variable* var) = 0; 
  virtual T visitAssign(Expr::Assign* expr) = 0; 
  virtual T visitVarStmt(Stmt::Var* stmt) = 0; 

  virtual T visitClassStmt(Stmt::Class* stmt) = 0;
  virtual T visitFunctionStmt(Stmt::Function* stmt) = 0;
  virtual T visitReturnStmt(Stmt::Return* stmt) = 0;
  virtual T visitCall(Expr::Call* expr) = 0; 
  virtual T visitGetExpr(Expr::Get* expr) = 0; 
  virtual T visitSetExpr(Expr::Set* expr) = 0; 
  virtual T visitThisExpr(Expr::This* expr) = 0; 
};
