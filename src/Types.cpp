#include "../include/Types.hpp"
#include "../include/Token.hpp"
#include "../include/LoxInstance.hpp"

#include <iostream>

Value::Value(const Value& other)
  : value{other.value} {}

Value::Value()
  : value{42.0f} {}


template <typename T>
T Value::get() const {
  return std::get<T>(value);
}

std::string Value::toString() const {
  if(std::holds_alternative<std::string>(value)) {
    return std::get<std::string>(value);
  } else if(std::holds_alternative<float>(value)) {
    float f = std::get<float>(value);
    int c;
    if(std::abs(f - int(f)) == 0.0) {
      int c = f;
      return std::to_string(c);
    }
    return std::to_string(f);
  } else if(std::holds_alternative<bool>(value)) {
    return std::get<bool>(value) ? "1" : "0";
  } else if(std::holds_alternative<std::shared_ptr<Callable>>(value)) {
    auto callable_ptr = std::get<std::shared_ptr<Callable>>(value);
    return callable_ptr->toString();
  } else if(std::holds_alternative<std::shared_ptr<LoxInstance>>(value)) {
    auto instance_ptr = std::get<std::shared_ptr<LoxInstance>>(value);
    return instance_ptr->toString();
  } else if(std::holds_alternative<Nil>(value)) {

    return "nil";
  }
  return "";
}


Literal::Literal(float v): value {v} {}
Literal::Literal(bool v): value {v} {}
Literal::Literal(const std::string& v): value {v} {}
Literal::Literal(Nil v): value {v} {}
Literal::Literal(std::shared_ptr<Callable> cal): value {cal} {}

template <typename T>
T Literal::get() const {
  return value.get<T>();
}

std::string Literal::toString() const {
  return value.toString();
}
