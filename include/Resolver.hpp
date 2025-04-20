#include <memory>
#include <map>

#include "Stmt.hpp"
#include "Expr.hpp"
#include "Visitor.hpp"
#include "Lox.hpp"
#include "Interpreter.hpp"

using Scope = std::map<std::string, bool>;

class Resolver : public Visitor {
  enum class FunctionType {
    NONE,
    FUNCTION,
    METHOD
  };

  enum class ClassType {
    NONE,
    CLASS,
  };

  Interpreter& interpreter;
  Lox* lox;
  std::vector<Scope> scopes;

  FunctionType currentFunction { FunctionType::NONE };
  ClassType currentClass { ClassType::NONE };
  bool inLoop;

  void resolveLocal(Expr::Expr* expr, Token name);
  void resolveFunction(Stmt::Function* function,
      FunctionType type);

  void beginScope();
  void endScope();

  void declare(Token name);
  void define(Token name);

public:
  Resolver(Interpreter& interpreter, Lox* lox);

  void resolve(Stmt::StmtPtr stmt);
  void resolve(Expr::ExprPtr expr);
  void resolve(Stmt::Stmts statements);

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

  virtual Value visitClassStmt(Stmt::Class* stmt) override;
  virtual Value visitCall(Expr::Call* expr) override; 
  virtual Value visitFunctionStmt(Stmt::Function* stmt) override;
  virtual Value visitReturnStmt(Stmt::Return* stmt) override;
  virtual Value visitGetExpr(Expr::Get* expr) override;
  virtual Value visitSetExpr(Expr::Set* expr) override; 
  virtual Value visitThisExpr(Expr::This* expr) override; 
};
