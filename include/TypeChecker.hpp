#pragma once

#include "Types.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Lox.hpp"
#include "Visitor.hpp"

#include <map>
#include <vector>
#include <optional>


class TypeChecker : public Visitor<Type> {
  using Scope = std::map<std::string, Type>;

  Lox& lox;
  std::vector<Scope> scopes {};
public:
  TypeChecker(Lox& lox); 
  ~TypeChecker();
  Type typeCheck(Expr::ExprPtr expr);
  Type typeCheck(Stmt::StmtPtr stmt);
  Type typeCheck(const Stmt::Stmts& program);

  void beginScope();
  void endScope();

  Type getVarType(const Token& varName);

  virtual Type visitBinop(Expr::Binop* expr) override; 
  virtual Type visitUnop(Expr::Unop* expr) override; 
  virtual Type visitGrouping(Expr::Grouping* expr) override; 
  virtual Type visitLiteralExpr(Expr::Literal* expr) override; 

  virtual Type visitExprStmt(Stmt::Expr* exprstmt) override; 
  virtual Type visitPrintStmt(Stmt::Print* varstmt) override; 
  virtual Type visitVarStmt(Stmt::Var* stmt) override; 

  virtual Type visitVariableExpr(Expr::Variable* var) override; 
  virtual Type visitAssign(Expr::Assign* expr) override; 
  virtual Type visitBlockStmt(Stmt::Block* stmt) override; 
  virtual Type visitIfStmt(Stmt::If* stmt) override; 
  virtual Type visitLogical(Expr::Logical* expr) override; 
  virtual Type visitWhileStmt(Stmt::While* stmt) override; 
  virtual Type visitBreakStmt(Stmt::Break* stmt) override; 

  virtual Type visitClassStmt(Stmt::Class* stmt) override;
  virtual Type visitCall(Expr::Call* expr) override; 
  virtual Type visitFunctionStmt(Stmt::Function* stmt) override;
  virtual Type visitReturnStmt(Stmt::Return* stmt) override;
  virtual Type visitGetExpr(Expr::Get* expr) override; 
  virtual Type visitSetExpr(Expr::Set* expr) override; 
  virtual Type visitThisExpr(Expr::This* expr) override;
};
