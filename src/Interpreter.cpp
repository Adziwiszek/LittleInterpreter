#include "../include/Interpreter.hpp"
#include "../include/NativeFunctions.hpp"
#include "../include/LoxFunction.hpp"

#include <iostream>

void Interpreter::checkNumberOperand(Token op, Value& val) const {
  if(std::holds_alternative<float>(val.value)) return;
  throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(Token op, Value& left, Value& right) const {
  if(std::holds_alternative<float>(left.value) &&
      std::holds_alternative<float>(right.value)) return;
  throw RuntimeError(op, "Operands must be a number.");
}

void Interpreter::reportDifferentTypesOperands() const {
  //throw RuntimeError(
}

bool Interpreter::isTruthy(Value val) const {
  return std::visit([](auto&& arg) -> bool {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, Nil>) {
      return false;  // nil is false
      } else if constexpr (std::is_same_v<T, bool>) {
      return arg;  // return the boolean value itself
      } else if constexpr (std::is_same_v<T, float>) {
      return arg != 0;  // nonzero numbers are true
      } else if constexpr (std::is_same_v<T, std::string>) {
      return !arg.empty();  // non-empty strings are true
      }
      return false; // Default case (shouldn't happen)
      }, val.value);
}
bool Interpreter::isEqual(Value left, Value right) const {
  if(std::holds_alternative<Nil>(left.value)) {
    return std::holds_alternative<Nil>(right.value);
  }
  auto eq = [&left, &right]<typename T>() -> bool {
    auto l = std::get<T>(left.value);
    auto r = std::get<T>(right.value);
    return l == r;
  };
  if(std::holds_alternative<std::string>(left.value) 
      && std::holds_alternative<std::string>(right.value)) 
    return eq.template operator()<std::string>();
  if(std::holds_alternative<float>(left.value)
      && std::holds_alternative<float>(right.value)) 
    return eq.template operator()<float>();
  if(std::holds_alternative<bool>(left.value)
      && std::holds_alternative<bool>(right.value)) 
    return eq.template operator()<bool>();
  //reportDifferentTypesOperands();
  // for now two different types are always not equal
  return false;
}
void Interpreter::executeBlock(const Stmts& statements,
    std::shared_ptr<Environment> env) {
  // tempenv sets current environment to the new one and in the
  // destructor restores it to the old one, using RAII
  TemporaryEnv tempenv = TemporaryEnv(this->environment, env);
  for(const auto& stmt: statements) {
    execute(stmt);
  }
}

Value Interpreter::visitBinop(Expr::Binop* expr) {
  Value left = evaluate(expr->left);
  Value right = evaluate(expr->right);

  auto executeBinop = 
    [&left, &right]<typename T, typename F>(F fn) -> T { 
      auto l = std::get<T>(left.value);
      auto r = std::get<T>(right.value);
      return fn(l, r);
    };

  switch (expr->op.type) {
    case EQUAL_EQUAL: return isEqual(left, right);
    case BANG_EQUAL: return !isEqual(left, right);
    case GREATER:
      checkNumberOperands(expr->op, left, right);
      return executeBinop.template operator()<float>(std::greater<float>{});
    case GREATER_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return executeBinop.template operator()<float>(std::greater_equal<float>{});
    case LESS:
      checkNumberOperands(expr->op, left, right);
      return executeBinop.template operator()<float>(std::less<float>{});
    case LESS_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return executeBinop.template operator()<float>(std::less_equal<float>{});
    case MINUS:
      checkNumberOperands(expr->op, left, right);
      return executeBinop.template operator()<float>(std::minus<float>{});
    case SLASH:
      checkNumberOperands(expr->op, left, right);
      return executeBinop.template operator()<float>(std::divides<float>{});
    case STAR:
      checkNumberOperands(expr->op, left, right);
      return executeBinop.template operator()<float>(std::multiplies<float>{});
    case PLUS:
      if(std::holds_alternative<float>(left.value) 
          && std::holds_alternative<float>(right.value)) {
        return executeBinop.template operator()<float>
          (std::plus<float>{});
      }

      if(std::holds_alternative<std::string>(left.value) 
          && std::holds_alternative<std::string>(right.value)) {
        auto l = std::get<std::string>(left.value);
        auto r = std::get<std::string>(right.value);
        return l + r;
      }
      throw RuntimeError(expr->op, "Operands must be two numbers or two strings");
    default: break;
  }
  return Nil();
}
Value Interpreter::visitUnop(Expr::Unop* expr) {
  Value right = evaluate(expr->expr);
  switch(expr->op.type) {
    case BANG: {
                 return !isTruthy(right);
               }
    case MINUS: {
                  checkNumberOperand(expr->op, right);
                  float val = std::get<float>(right.value);
                  return -val;
                }
    case PLUSPLUS: {
                     checkNumberOperand(expr->op, right);
                     float val = std::get<float>(right.value);
                     return ++val;
                   }
    case MINUSMINUS: {
                       checkNumberOperand(expr->op, right);
                       float val = std::get<float>(right.value);
                       return --val;
                     }
    default: break;
  }
  return Nil();
}
Value Interpreter::visitGrouping(Expr::Grouping* expr) {
  return evaluate(expr->expr);
}
Value Interpreter::visitLiteralExpr(Expr::Literal* expr) {
  return expr->value->value;
}
Value Interpreter::visitExprStmt(Stmt::Expr* exprstmt) {
  evaluate(exprstmt->expr);
  return Nil();
}
Value Interpreter::visitPrintStmt(Stmt::Print* varstmt) {
  Value res = evaluate(varstmt->expr);
  std::cout << res.toString() << std::endl;
  return Nil();
}
Value Interpreter::visitVarStmt(Stmt::Var* stmt) {
  Value val = Nil();
  if(stmt->initializer) {
    val = evaluate(stmt->initializer);
  }
  environment->define(stmt->name.lexeme, val);
  return val;
}
Value Interpreter::visitVariableExpr(Expr::Variable* expr) {
  return lookUpVariable(expr->name, expr);
}
Value Interpreter::visitAssign(Expr::Assign* expr) {
  Value value = evaluate(expr->value);

  if(locals.contains(expr)) {
    int distance = locals[expr];
    environment->assignAt(distance, expr->name, value);
  } else {
    globals->assign(expr->name, value);
  }

  return value;
}
Value Interpreter::visitBlockStmt(Stmt::Block* stmt) {
  executeBlock(stmt->statements, std::make_shared<Environment>(environment));
  return Nil();
}
Value Interpreter::visitIfStmt(Stmt::If* stmt) {
  if(isTruthy(evaluate(stmt->condition))) {
    execute(stmt->thenBranch);
  } else if(stmt->elseBranch) {
    execute(stmt->elseBranch);
  }
  return Nil();
}
Value Interpreter::visitLogical(Expr::Logical* expr) {
  Value left = evaluate(expr->left);
  if(expr->op.type == OR) {
    if(isTruthy(left)) return left;
  } else if(expr->op.type == AND) {
    if(!isTruthy(left)) return left;
  }
  return evaluate(expr->right);
}
Value Interpreter::visitWhileStmt(Stmt::While* stmt) {
  while(isTruthy(evaluate(stmt->condition))) {
    try {
      execute(stmt->body);
    } catch(BreakLoop e) {
      break;
    }
  }
  return Nil();
}
Value Interpreter::visitBreakStmt(Stmt::Break* stmt) {
  throw BreakLoop();
}
Value Interpreter::visitCall(Expr::Call* expr) {
  Value callee = evaluate(expr->callee);

  std::vector<Value> args {};
  for(const auto& arg: expr->arguments) {
    args.push_back(evaluate(arg));
  }

  if(auto func = std::get_if<std::shared_ptr<Callable>>(&callee.value)) {
    if(*func) {
      if((*func)->arity() != args.size()) {
        throw RuntimeError(expr->paren, 
            "Expected " + std::to_string((*func)->arity()) +
            " arguments, but got " + std::to_string(args.size()) + " instead.");
      }
      return (*func)->call(this, args);
    }
  }
  throw RuntimeError(expr->paren, "Can only call functions and classes");
}

Value Interpreter::visitFunctionStmt(Stmt::Function* stmt) {
  // converting normal pointer to shared_ptr
  std::shared_ptr<Stmt::Function> fstmt =
    std::make_shared<Stmt::Function>(*stmt);
  // making callable lox function
  std::shared_ptr<Callable> calfun =
    std::make_shared<LoxFunction>(fstmt, environment);
  // creating value that holds that lox function
  Value valfun = Value(calfun);
  // defining function in environment
  environment->define(stmt->name.lexeme, valfun);
  return Nil();
}

Value Interpreter::visitReturnStmt(Stmt::Return* stmt) {
  Value value = Nil();
  if(stmt->value) value = evaluate(stmt->value); 
  throw Return(value);
}

Interpreter::Interpreter(std::shared_ptr<Lox> lox) 
  : globals { std::make_shared<Environment>() }, lox { std::move(lox) }
{
  std::shared_ptr<Native::Clock> clock = std::make_shared<Native::Clock>();
  globals->define("clock", clock);
  environment = globals;
}

Value Interpreter::lookUpVariable(Token name, Expr::Expr* expr) {
  if(locals.contains(expr)) {
    return environment->getAt(locals[expr], name.lexeme);
  } else {
    return globals->get(name);
  }
}

Value Interpreter::evaluate(const ExprPtr& expr) {
  return expr->accept(this);
}

void Interpreter::execute(const StmtPtr& stmt) {
  stmt->accept(this);
}

void Interpreter::resolve(Expr::Expr* expr, int depth) {
  locals.insert({expr, depth});
}
void resolve(Expr::Expr* expr, int depth);

void Interpreter::interpret(const Stmts& program) {
  try {
    for(const auto& stmt: program) {
      execute(stmt);
    }
  } catch(RuntimeError error) {
    lox->runtimeError(error); 
  }
}
