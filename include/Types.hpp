#pragma once


#include <variant>
#include <vector>
#include <string>
#include <memory>


// Basic Types
enum class Type {
  NIL,
  BOOLEAN,
  NUMBER,
  STRING,
  FUNCTION,
};

inline std::string typeToString(Type type) {
  switch (type) {
    case Type::NIL: return "nil";
    case Type::BOOLEAN: return "boolean";
    case Type::NUMBER: return "number";
    case Type::STRING: return "string";
    case Type::FUNCTION: return "function";
    //case Type::CLASS: return "class";
    //case Type::INSTANCE: return "instance";
    default: return "Unknown Type!";
  }
}

// forward declarations
class Interpreter;
class Value;
class LoxInstance;
class Token;
using Tokens = std::vector<Token>;

// Used for representing NULL value in my language
class Nil {public: Nil() {} };

class Callable {
public:
  virtual int arity() = 0;
  virtual Value call(Interpreter* interpreter, std::vector<Value> args) = 0;
  virtual std::string toString() = 0;
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
    std::shared_ptr<Callable>,
    std::shared_ptr<LoxInstance>
  > value;

  Value();
  Value(const Value& other);

  template <typename T>
  Value& operator=(const T& val) {
    value = val;
    return *this;
  }

  template <typename T>
  Value(const T& val)
  {
    value = val;
  }

  template <typename T>
  T get() const;

  std::string toString() const; 

  Type getType() const;
  bool isType(Type type) const;
  std::string getTypeName() const;
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

