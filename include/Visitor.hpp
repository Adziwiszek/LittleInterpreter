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

  virtual Value visitClassStmt(Stmt::Class* stmt) = 0;
  virtual Value visitFunctionStmt(Stmt::Function* stmt) = 0;
  virtual Value visitReturnStmt(Stmt::Return* stmt) = 0;
  virtual Value visitCall(Expr::Call* expr) = 0; 
  virtual Value visitGetExpr(Expr::Get* expr) = 0; 
};

/*class AstPrinter : public Visitor {
public:
  std::string print(ExprPtr expr) {
    std::any result = expr->accept(this);
    if(result.type() == typeid(std::string)) {
      return std::any_cast<string>(result);
    } else {
      throw std::runtime_error("Unexpected return type from accept \
          in AstPrinter!!!");
    }
  }
  /*static string print_(std::shared_ptr<Expr> expr) {
    auto pp = new AstPrinter();
    Value result = expr->accept(pp);
    std::cout << valueToString(result) << std::endl;
    return valueToString(result);
    }
  virtual Value visitBinop(Binop* binop) override {
    return parenthesize(binop->op.lexeme, binop->left, binop->right);
  }
  virtual Value visitUnop(Unop* unop) override {
    return parenthesize(unop->op.lexeme, unop->expr);        
  }
  virtual Value visitGrouping(Grouping* expr) override {
    return parenthesize("group", expr->expr);
  }
  virtual Value visitLiteralExpr(LiteralExpr* expr) override {
    if(!expr->value) return "nil";
    return expr->value->toString();
  }

  template <typename... Exprs>
    void processExprsHelper(std::ostringstream& oss, 
        const std::shared_ptr<Exprs>&... exprs) {
      ((oss << " " << std::any_cast<std::string>(exprs->accept(this))), ...);
    }
  template <typename... Exprs>
    string parenthesize(const string& name, 
        const std::shared_ptr<Exprs>&... exprs) {
      std::ostringstream oss;
      oss << "(" << name;
      processExprsHelper(oss, exprs...);
      oss << ")";
      return oss.str();
    }
  virtual Value visitAssign(Assign* expr) override {
    return parenthesize(expr->name.lexeme, expr->value);
  }
  virtual Value visitVariableExpr(VariableExpr* expr) override {
    return parenthesize(expr->name.lexeme);
  }
  virtual Value visitLogical(Logical* expr) override {
    return parenthesize(expr->op.lexeme, expr->left, expr->right);
  }

  virtual Value visitExprStmt(ExprStmt* exprstmt) override { return Nil(); }
  virtual Value visitPrintStmt(PrintStmt* print) override { return Nil(); }
  virtual Value visitVarStmt(VarStmt* var) override { return Nil(); }
  virtual Value visitBlockStmt(BlockStmt* block) override { return Nil(); }
  virtual Value visitIfStmt(IfStmt* stmt) override { return Nil(); }
  virtual Value visitWhileStmt(WhileStmt* stmt) override { return Nil(); }
  virtual Value visitBreakStmt(BreakStmt* stmt) override { return Nil(); }
};*/
