#pragma once

#include <string>
#include <map>

#include "LoxClass.hpp"

class LoxInstance {
  LoxClass* klass;
  std::map<std::string, Value> fields;
public:
  LoxInstance(LoxClass* klass);
  LoxInstance(const LoxInstance& other);
  Value get(Token fieldName);
  void set(Token fieldName, Value value);
  std::string toString();
};
