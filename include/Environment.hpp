#pragma once

#include <map>
#include <memory>
#include <string>

#include "Types.hpp"
#include "Token.hpp"

class Environment {
  std::map<std::string, Value> values;
  std::shared_ptr<Environment> enclosing;
public:
  Environment(std::shared_ptr<Environment> enclosing);
  Environment();

  void define(std::string name, Value value);
  Value get(const Token& name) const;
  void assign(Token name, Value value);
};

class TemporaryEnv {
  std::shared_ptr<Environment>& env;
  std::shared_ptr<Environment> prevEnv;
  public:
  TemporaryEnv(std::shared_ptr<Environment>& currentEnv,
      std::shared_ptr<Environment> newEnv);
  ~TemporaryEnv(); 
};
