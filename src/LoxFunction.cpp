#include "../include/LoxFunction.hpp"
#include "../include/Interpreter.hpp"

LoxFunction::LoxFunction(std::shared_ptr<Stmt::Function> declaration)
  : declaration{declaration} {}

int LoxFunction::arity() {
  return declaration->args.size();
}

Value LoxFunction::call(Interpreter* interpreter, std::vector<Value> args) {
  std::shared_ptr<Environment> env =
    std::make_shared<Environment>(interpreter->globals);

  for(size_t i = 0; i < declaration->args.size(); i++) {
    env->define(declaration->args[i].lexeme, args[i]);
  }

  interpreter->executeBlock(declaration->body, env);
  return Nil();
}
