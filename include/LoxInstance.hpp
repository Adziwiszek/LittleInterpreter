#pragma once

#include <string>
#include <map>

#include "LoxClass.hpp"

class LoxInstance {
  LoxClass* klass;
  std::map<std::string, Value> fields;
public:
  LoxInstance(LoxClass* klass);
  Value get(Token fieldName);
  std::string toString();
};
