#include "../include/LoxInstance.hpp"
#include "../include/LoxFunction.hpp"

#include <iostream>

LoxInstance::LoxInstance(LoxClass *klass) : klass{klass} {}

LoxInstance::LoxInstance(const LoxInstance &other)
    : klass{other.klass}, fields{other.fields} {}

std::string LoxInstance::toString() { return klass->name + " instance"; }

Value LoxInstance::get(Token fieldName) {
  if (fields.contains(fieldName.lexeme)) {
    return fields[fieldName.lexeme];
  }

  FunPtr method = klass->findMethod(fieldName.lexeme);
  if (method)
    return method->bind(this);

  throw RuntimeError(fieldName,
                     "Undefined property '" + fieldName.lexeme + "'.");
}

void LoxInstance::set(Token fieldName, Value value) {
  fields[fieldName.lexeme] = value;
}
