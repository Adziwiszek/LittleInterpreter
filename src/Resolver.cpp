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

void Resolver::resolveFunction(Stmt::Function* function,
    FunctionType type) {
  FunctionType enclosingFunction = currentFunction;
  currentFunction = type;

  beginScope();
  for(const auto& param: function->args) {
    declare(param);
    define(param);
  }
  resolve(function->body);
  endScope();

  currentFunction = enclosingFunction;
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
  Scope& scope = scopes.back();
  if(scope.contains(name.lexeme)) {
    lox->error(name,
        "Already a variable with this name in this scope");
  }
  scope[name.lexeme] = false;
}

void Resolver::define(Token name) {
  if(scopes.empty()) return;
  DEBPRINT("Defining: " + name.lexeme);
  scopes.back()[name.lexeme] = true;
}

Resolver::Resolver(Interpreter& interpreter, Lox* lox)
  : interpreter { interpreter }, scopes {}, lox {lox},
   inLoop {false} {}


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
  bool enclosingLoopState = inLoop;
  inLoop = true;

  resolve(stmt->condition);
  resolve(stmt->body);

  inLoop = enclosingLoopState;
  return Nil();
}

Value Resolver::visitBreakStmt(Stmt::Break* stmt) {
  if(!inLoop) {
    lox->error(stmt->keyword, "'break' statement outside of a loop.");
  }
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

  resolveFunction(stmt, FunctionType::FUNCTION);
  return Nil();
}

Value Resolver::visitReturnStmt(Stmt::Return* stmt) {
  if(currentFunction == FunctionType::NONE) {
    lox->error(stmt->keyword, "Return statement outside of a function.");
  }
  if(stmt->value) resolve(stmt->value);
  return Nil();
}

Value Resolver::visitClassStmt(Stmt::Class* stmt) {
  ClassType enclosingClass = currentClass;
  currentClass = ClassType::CLASS;

  declare(stmt->name);
  define(stmt->name);

  beginScope();
  scopes.back().insert({"this", true});

  for(auto& method: stmt->methods) {
    FunctionType declaration = FunctionType::METHOD;
    resolveFunction(method.get(), declaration);
  }

  endScope();
  currentClass = enclosingClass;

  return Nil();
}

Value Resolver::visitGetExpr(Expr::Get* expr) {
  resolve(expr->object);
  return Nil();
}
Value Resolver::visitSetExpr(Expr::Set* expr) {
  resolve(expr->object);
  resolve(expr->value);
  return Nil();
}

Value Resolver::visitThisExpr(Expr::This* expr) {
  if(currentClass == ClassType::NONE) {
    lox->error(expr->keyword, "Can't use 'this' outside of a class.");
    return Nil();
  }

  resolveLocal(expr, expr->keyword);
  return Nil();
}
