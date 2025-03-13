#include "../include/Resolver.hpp"

#include <iostream>

//#define DEBUG

#ifdef DEBUG
  #define DEBPRINT(x) std::cout << x << "\n";
#else
  #define DEBPRINT(x)
#endif

void Resolver::resolve(Expr::ExprPtr expr) {
  expr->accept(this);
}

void Resolver::resolve(Stmt::StmtPtr stmt) {
  stmt->accept(this);
}

void Resolver::resolve(Stmt::Stmts statements) {
  for(const auto& statement: statements) {
    resolve(statement);
  }
}

void Resolver::resolveLocal(Expr::Expr* expr, Token name) {
  for(int i = scopes.size() - 1; i >= 0; i--) {
    if(scopes.at(i).contains(name.lexeme)) {
      interpreter.resolve(expr, scopes.size() - 1 - i);
      return;
    }
  }
}

void Resolver::resolveFunction(Stmt::Function* function) {
  beginScope();
  for(const auto& param: function->args) {
    declare(param);
    define(param);
  }
  resolve(function->body);
  endScope();
}

void Resolver::beginScope() {
  DEBPRINT("creating new scope!");
  scopes.push_back(Scope());
}

void Resolver::endScope() {
  DEBPRINT("closing scope!");
  scopes.pop_back();
}

void Resolver::declare(Token name) {
  if(scopes.empty()) return;
  DEBPRINT("Declaring: " + name.lexeme);
  scopes.back()[name.lexeme] = false;
}

void Resolver::define(Token name) {
  if(scopes.empty()) return;
  DEBPRINT("Defining: " + name.lexeme);
  scopes.back()[name.lexeme] = true;
}

Resolver::Resolver(Interpreter& interpreter, Lox* lox)
  : interpreter { interpreter }, scopes {}, lox {lox} {}


Value Resolver::visitBinop(Expr::Binop* expr) {
  resolve(expr->left);
  resolve(expr->right);
  return Nil();
}

Value Resolver::visitUnop(Expr::Unop* expr) {
  resolve(expr->expr);
  return Nil();
}

Value Resolver::visitGrouping(Expr::Grouping* expr) {
  resolve(expr->expr);
  return Nil();
}

Value Resolver::visitLiteralExpr(Expr::Literal* expr) {
  return Nil();
}

Value Resolver::visitExprStmt(Stmt::Expr* exprstmt) {
  resolve(exprstmt->expr);
  return Nil();
}

Value Resolver::visitPrintStmt(Stmt::Print* stmt) {
  resolve(stmt->expr);
  return Nil();
}

Value Resolver::visitVarStmt(Stmt::Var* stmt) {
  declare(stmt->name);
  if(stmt->initializer) {
    resolve(stmt->initializer);
  }
  define(stmt->name);
  return Nil();
}

Value Resolver::visitVariableExpr(Expr::Variable* var) {
  if(!scopes.empty() && scopes.back().contains(var->name.lexeme) 
     && !scopes.back()[var->name.lexeme]) {
    lox->error(var->name, "Can't read local variable in its own initializer."); 
  } else {
    resolveLocal(var, var->name);
  }
  return Nil();
}

Value Resolver::visitAssign(Expr::Assign* expr) {
  resolve(expr->value);
  resolveLocal(expr, expr->name);
  return Nil();
}

Value Resolver::visitBlockStmt(Stmt::Block* stmt) {
  beginScope();
  resolve(stmt->statements);  
  endScope();
  return Nil();
}

Value Resolver::visitIfStmt(Stmt::If* stmt) {
  resolve(stmt->condition);
  resolve(stmt->thenBranch);
  if(stmt->elseBranch) resolve(stmt->elseBranch);
  return Nil();
}

Value Resolver::visitLogical(Expr::Logical* expr) {
  resolve(expr->left);
  resolve(expr->right);
  return Nil();
}

Value Resolver::visitWhileStmt(Stmt::While* stmt) {
  resolve(stmt->condition);
  resolve(stmt->body);
  return Nil();
}

Value Resolver::visitBreakStmt(Stmt::Break* stmt) {
  return Nil();
}

Value Resolver::visitCall(Expr::Call* expr) {
  resolve(expr->callee);
  for(const auto& arg: expr->arguments) {
    resolve(arg);
  }
  return Nil();
}

Value Resolver::visitFunctionStmt(Stmt::Function* stmt) {
  declare(stmt->name);
  define(stmt->name);

  resolveFunction(stmt);
  return Nil();
}

Value Resolver::visitReturnStmt(Stmt::Return* stmt) {
  if(stmt->value) resolve(stmt->value);
  return Nil();
}
