#pragma once

#include <variant>
#include <vector>
#include <string>
#include <memory>

// forward declarations
class Interpreter;
class Value;

// Used for representing NULL value in my language
class Nil {public: Nil() {} };

class Callable {
public:
  virtual int arity() = 0;
  virtual Value call(Interpreter* interpreter, std::vector<Value> args) = 0;
  virtual ~Callable() = default;
};

// Values used in my language
class Value {
public:
  std::variant<
    float, 
    bool, 
    std::string, 
    Nil, 
    std::shared_ptr<Callable>
  > value;

  Value(const Value& other);
  Value(float f);
  Value(bool b);
  Value(const std::string& s);
  Value(Nil v);
  Value(std::shared_ptr<Callable> cal);
  std::string toString(); 
};

struct Literal {
  Value value;

  Literal(float v);
  Literal(bool v);
  Literal(const std::string& v);
  Literal(Nil v);
  Literal(std::shared_ptr<Callable> cal);
  template <typename T>
  T get() const; 
  std::string toString() const; 
};
using LiteralPtr = std::shared_ptr<Literal>;

// Helper types
