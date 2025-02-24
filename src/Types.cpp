#include "../include/Types.hpp"


Value::Value(const Value& other)
  : value{other.value} {}
Value::Value(float f)
  : value{f} {}
Value::Value(bool b)
  : value{b} {}
Value::Value(const std::string& s)
  : value{s} {}
Value::Value(Nil v)
  : value{v} {}
Value::Value(std::shared_ptr<Callable> cal)
  : value{cal} {}

std::string Value::toString() {
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
  return std::get<T>(value);
}

std::string Literal::toString() const {
  if(std::holds_alternative<std::string>(value)) {
    return get<std::string>();
  } else if(std::holds_alternative<float>(value)) {
    return std::to_string(get<float>());
  } else if(std::holds_alternative<bool>(value)) {
    return get<bool>() ? "1" : "0";
  } else if(std::holds_alternative<Nil>(value)) {
    return "nil";
  }
  return "";
}
