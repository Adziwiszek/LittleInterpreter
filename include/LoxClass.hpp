#pragma once

#include <string>

class LoxClass {
public:
  std::string name;

  LoxClass(const std::string& name);
  std::string toString();
};
