#pragma once

#include <string>
#include "LoxClass.hpp"

class LoxInstance {
  LoxClass* klass;
public:
  LoxInstance(LoxClass* klass);
  std::string toString();
};
