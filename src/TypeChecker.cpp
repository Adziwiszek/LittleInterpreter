#include "../include/TypeChecker.hpp"
#include "../include/TypeChecker.hpp"

#include <iostream>

TypeChecker::TypeChecker(Lox& lox)
  : lox {lox} {}

TypeChecker::~TypeChecker() {}

Type TypeChecker::typeCheck(Expr::ExprPtr expr) {
  return expr->accept(this);
}

Type TypeChecker::typeCheck(Stmt::StmtPtr stmt) {
  stmt->accept(this);
  return Type::NIL;
}

Type TypeChecker::typeCheck(const Stmt::Stmts& program) {
  for(const auto& stmt: program) {
    typeCheck(stmt);
  }
  return Type::NIL;
}

Type TypeChecker::visitBinop(Expr::Binop* expr) { 
  Type typeLeft = typeCheck(expr->left);
  Type typeRight = typeCheck(expr->right);
  if(typeLeft != typeRight) {
    lox.error(expr->op, "Cannot do " + expr->op.toString() + "between " +
        typeToString(typeLeft) + " and " + typeToString(typeRight)); 
  }
  return {}; 
}

Type TypeChecker::visitUnop(Expr::Unop* expr) { 
  Type exprType = typeCheck(expr->expr);
  return exprType; 
}
Type TypeChecker::visitGrouping(Expr::Grouping* expr) { return {}; }
Type TypeChecker::visitLiteralExpr(Expr::Literal* expr) { 
  return expr->value->value.getType(); 
}

Type TypeChecker::visitExprStmt(Stmt::Expr* exprstmt) { return {}; }
Type TypeChecker::visitPrintStmt(Stmt::Print* varstmt) { 
  typeCheck(varstmt->expr);
  return {}; 
}
Type TypeChecker::visitVarStmt(Stmt::Var* stmt) { return {}; }

Type TypeChecker::visitVariableExpr(Expr::Variable* var) { return {}; }
Type TypeChecker::visitAssign(Expr::Assign* expr) { return {}; }
Type TypeChecker::visitBlockStmt(Stmt::Block* stmt) { 
  for(const auto& stmt: stmt->statements) {
    typeCheck(stmt);
  }
  return Type::NIL; 
}
Type TypeChecker::visitIfStmt(Stmt::If* stmt) { 
  Type condType = typeCheck(stmt->condition);
  if(condType != Type::BOOLEAN) {
    lox.error(stmt->line, "Condition must be of boolean type."); 
  }

  if(stmt->thenBranch) {
    typeCheck(stmt->thenBranch);
  }
  if(stmt->elseBranch)
    typeCheck(stmt->elseBranch);
  return Type::NIL; 
}
Type TypeChecker::visitLogical(Expr::Logical* expr) { return {}; }
Type TypeChecker::visitWhileStmt(Stmt::While* stmt) { return {}; }
Type TypeChecker::visitBreakStmt(Stmt::Break* stmt) { return {}; }

Type TypeChecker::visitClassStmt(Stmt::Class* stmt) { return {}; }
Type TypeChecker::visitCall(Expr::Call* expr) { return {}; }
Type TypeChecker::visitFunctionStmt(Stmt::Function* stmt) { return {}; }
Type TypeChecker::visitReturnStmt(Stmt::Return* stmt) { return {}; }
Type TypeChecker::visitGetExpr(Expr::Get* expr) { return {}; }
Type TypeChecker::visitSetExpr(Expr::Set* expr) { return {}; }
Type TypeChecker::visitThisExpr(Expr::This* expr) { return {}; }
