#include "Environment.hpp"
#include "Stmt.hpp"
#include "Types.hpp"

class LoxFunction;
using FunPtr = std::shared_ptr<LoxFunction>;

class LoxFunction : public Callable {
  std::shared_ptr<Stmt::Function> declaration;
  std::shared_ptr<Environment> closure;

public:
  LoxFunction(std::shared_ptr<Stmt::Function> declaration,
              std::shared_ptr<Environment> env);
  virtual std::string toString() override;
  virtual int arity() override;
  virtual Value call(Interpreter *interpreter,
                     std::vector<Value> args) override;
  FunPtr bind(LoxInstance* instance);  
};
