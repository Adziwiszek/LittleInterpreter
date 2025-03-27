#pragma once

#include <map>
#include <string>

#include "Types.hpp"
#include "LoxFunction.hpp"
#include "Interpreter.hpp"

class LoxClass : public Callable {
public:
  std::string name;
  std::map<std::string, FunPtr> methods;

  LoxClass(const std::string& name, const std::map<std::string, FunPtr>& methods);

  FunPtr findMethod(const std::string& name);
  virtual std::string toString() override;
  virtual int arity() override;
  virtual Value call(Interpreter* interpreter, std::vector<Value> args) override;
};
