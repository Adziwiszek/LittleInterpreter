#pragma once

#include <vector>
#include <memory>

#include "Token.hpp"
#include "Expr.hpp"

using Expr::ExprPtr;

namespace Stmt {

class Stmt {
  public:
    virtual ~Stmt() = default;
    virtual Value accept(Visitor* visitor) = 0;
};

using StmtPtr = std::shared_ptr<Stmt>;
using Stmts   = std::vector<StmtPtr>;

class Return : public Stmt {
public:
  Token keyword;
  ExprPtr value;

  Return(const Token& keyword, const ExprPtr& value);
  virtual Value accept(Visitor* visitor) override; 
};

class Function : public Stmt {
public:
  Token name;
  Tokens args;
  Stmts body;

  Function(Token name, const Tokens& args, const Stmts& body);
  Function(const Function& other);
  virtual Value accept(Visitor* visitor) override; 
};

class Break : public Stmt {
public:
  Break();
  virtual Value accept(Visitor* visitor) override; 
};

class While : public Stmt {
public:
  ExprPtr condition;
  std::shared_ptr<Stmt> body;

  While(std::shared_ptr<Expr::Expr> condition, std::shared_ptr<Stmt> body);
  virtual Value accept(Visitor* visitor) override; 
};

class If : public Stmt {
public:
  ExprPtr condition;
  StmtPtr thenBranch;
  StmtPtr elseBranch;

  If(ExprPtr& condition, StmtPtr& thenBranch, StmtPtr& elseBranch);
  virtual Value accept(Visitor* visitor) override; 
};


class Block : public Stmt {
public:
  Stmts statements;

  Block();
  Block(const Stmts& otherStatements);
  Block(const Block& other);

  template <typename... Stmts>
  Block(const std::shared_ptr<Stmts>&... stmts) : Block() {
    ((statements.push_back(std::move(stmts))), ...);
  }

  virtual Value accept(Visitor* visitor) override; 
};

class Expr : public Stmt {
public:
  ExprPtr expr;

  Expr(ExprPtr expr);
  virtual Value accept(Visitor* visitor) override; 
};

class Print : public Stmt {
public:
  ExprPtr expr;

  Print(ExprPtr expr);
  virtual Value accept(Visitor* visitor) override; 
};

class Var : public Stmt {
public:
  ExprPtr initializer;
  Token name;

  Var(ExprPtr expr, Token name);
  virtual Value accept(Visitor* visitor) override; 
};

}; // namespace Stmt
   
