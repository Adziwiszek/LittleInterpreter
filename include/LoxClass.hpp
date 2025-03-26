#pragma once

#include <string>
#include "Types.hpp"
#include "Interpreter.hpp"

class LoxClass : public Callable {
public:
  std::string name;

  LoxClass(const std::string& name);

  virtual std::string toString() override;
  virtual int arity() override;
  virtual Value call(Interpreter* interpreter, std::vector<Value> args) override;
};
