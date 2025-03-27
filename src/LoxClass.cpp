#include "../include/LoxClass.hpp"
#include "../include/LoxInstance.hpp"
#include <iostream>

LoxClass::LoxClass(const std::string& name, const std::map<std::string, FunPtr>& methods) :
  name{name}, methods{methods} {}

FunPtr LoxClass::findMethod(const std::string& name) {
  if(methods.contains(name)) {
    return methods[name];
  }
  return nullptr;
}

std::string LoxClass::toString() { 
  std::cout << "dupa klasa\n";
  return name; 
}

int LoxClass::arity() { return 0; }

Value LoxClass::call(Interpreter* interpreter, std::vector<Value> args) {
  LoxInstance instance = LoxInstance(this);
  //return instance;
  return {std::make_shared<LoxInstance>(instance)};
}
