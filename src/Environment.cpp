#include "../include/Environment.hpp"
#include "../include/Lox.hpp"

Environment::Environment(std::shared_ptr<Environment> enclosing) 
  : values {}, enclosing(std::move(enclosing)) {}

Environment::Environment() : enclosing(nullptr) { }

void Environment::define(std::string name, Value value) {
  values[name] = value;
}

Value Environment::get(const Token& name) const {
  if(values.contains(name.lexeme)) {
    return values.at(name.lexeme);
  }
  if(enclosing) return enclosing->get(name);
  throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'."); 
}

void Environment::assign(Token name, Value value) {
  if(values.contains(name.lexeme)) {
    values[name.lexeme] = value;
    return;
  }
  if(enclosing) {
    enclosing->assign(name, value);
    return;
  }
  throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

TemporaryEnv::TemporaryEnv(std::shared_ptr<Environment>& currentEnv,
    std::shared_ptr<Environment> newEnv)
  : env{ currentEnv }, prevEnv{ currentEnv } {
    env = newEnv;
  }
TemporaryEnv::~TemporaryEnv() {
  env = prevEnv;
}
