#include "../include/TypeChecker.hpp"
#include "../include/TypeChecker.hpp"

#include <iostream>

#define DEBUG

#ifdef DEBUG
  #define DPRINT(...) printf(__VA_ARGS__)
#else
  #define DPRINT(...)
#endif

TypeChecker::TypeChecker(Lox& lox)
  : lox {lox} 
{
  // adding global scope to type checker
  scopes.push_back({});
}

TypeChecker::~TypeChecker() {}

void TypeChecker::beginScope() {
  scopes.push_back(Scope());
}

void TypeChecker::endScope() {
  scopes.pop_back();
}

Type TypeChecker::getVarType(const Token& name) {
  for(int i = scopes.size() - 1; i >= 0; i--) {
    auto& currentScope = scopes[i];
    if(currentScope.contains(name.lexeme)) {
      return currentScope[name.lexeme];
    }
  }
  return Type::NIL;
}

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
  DPRINT("left = %s | right = %s\n", typeToString(typeLeft).c_str(), typeToString(typeRight).c_str());
  if(typeLeft != typeRight) {
    lox.error(expr->op, "Cannot do " + expr->op.toString() + "between " +
        typeToString(typeLeft) + " and " + typeToString(typeRight)); 
  }
  return {}; 
}

Type TypeChecker::visitUnop(Expr::Unop* expr) { 
  Type exprType = typeCheck(expr->expr);
  // ++, -- aren't here, won't be in my language in the future TODO
  switch(expr->op.type) {
    case MINUS:
      if(exprType != Type::NUMBER) lox.error(expr->op.line, 
          "Expression after '-' should have type Number.");
      break;
    case BANG:
      if(exprType != Type::BOOLEAN) lox.error(expr->op.line, 
          "Expression after '!' should have type Boolean.");
      break;
    // this shouldn't happen
    default:
      break;
  }
  return exprType; 
}

Type TypeChecker::visitGrouping(Expr::Grouping* expr) { 
  return typeCheck(expr->expr); 
}

Type TypeChecker::visitLiteralExpr(Expr::Literal* expr) { 
  return expr->value->value.getType(); 
}

Type TypeChecker::visitExprStmt(Stmt::Expr* exprstmt) { 
  return typeCheck(exprstmt->expr); 
}

Type TypeChecker::visitPrintStmt(Stmt::Print* varstmt) { 
  typeCheck(varstmt->expr);
  return Type::NIL; 
}

Type TypeChecker::visitVarStmt(Stmt::Var* stmt) { 
  auto& currentScope = scopes.back();
  currentScope[stmt->name.lexeme] = typeCheck(stmt->initializer);
  DPRINT("putting stuff into scope, %s\n", 
      typeToString(currentScope[stmt->name.lexeme]).c_str());
  return Type::NIL;
}

Type TypeChecker::visitVariableExpr(Expr::Variable* var) { 
  Type t = getVarType(var->name);
  //DPRINT("var expr type = %s\n", typeToString(t).c_str());
  return t; 
}

Type TypeChecker::visitAssign(Expr::Assign* expr) { return {}; }

Type TypeChecker::visitBlockStmt(Stmt::Block* stmt) { 
  beginScope();
  for(const auto& stmt: stmt->statements) {
    typeCheck(stmt);
  }
  endScope();
  return Type::NIL; 
}
Type TypeChecker::visitIfStmt(Stmt::If* stmt) { 
  Type condType = typeCheck(stmt->condition);
  if(condType != Type::BOOLEAN) {
    lox.error(stmt->line, "Condition must be of boolean type."); 
  }

  typeCheck(stmt->thenBranch);
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
