#include "../include/LoxInstance.hpp"

LoxInstance::LoxInstance(LoxClass* klass): klass { klass } {}

std::string LoxInstance::toString() {
  return klass->name + " instance";
}

Value LoxInstance::get(Token fieldName) {
  if(fields.contains(fieldName.lexeme)) {
    return fields[fieldName.lexeme];
  }

  throw RuntimeError(fieldName, "Undefined property '" + fieldName.lexeme + "'.");
}
