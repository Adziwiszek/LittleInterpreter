#include "../include/LoxFunction.hpp"
#include "../include/Interpreter.hpp"
#include "../include/LoxInstance.hpp"

LoxFunction::LoxFunction(std::shared_ptr<Stmt::Function> declaration, 
    std::shared_ptr<Environment> env)
  : declaration{ declaration }, closure { std::move(env) } {}

std::string LoxFunction::toString() {
  return "fun type";
}

int LoxFunction::arity() {
  return declaration->args.size();
}

Value LoxFunction::call(Interpreter* interpreter, std::vector<Value> args) {
  std::shared_ptr<Environment> env = std::make_shared<Environment>(closure);

  for(size_t i = 0; i < declaration->args.size(); i++) {
    env->define(declaration->args[i].lexeme, args[i]);
  }

  try {
    interpreter->executeBlock(declaration->body, env);
  } catch(Return retval) {
    return retval.value;
  }
  return Nil();
}

FunPtr LoxFunction::bind(LoxInstance* instance) {
  std::shared_ptr<Environment> env = std::make_shared<Environment>(closure);
  std::shared_ptr<LoxInstance> thisPtr = 
    std::shared_ptr<LoxInstance>(instance, [](LoxInstance*){});
  env->define("this", thisPtr);
  return std::make_shared<LoxFunction>(declaration, env);
}
