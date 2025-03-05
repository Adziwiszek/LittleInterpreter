#include "Types.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"

class LoxFunction : public Callable {
  std::shared_ptr<Stmt::Function> declaration;
  std::shared_ptr<Environment> closure; 
public:
  LoxFunction(std::shared_ptr<Stmt::Function> declaration, 
      std::shared_ptr<Environment> env);
  virtual int arity() override;
  virtual Value call(Interpreter* interpreter, std::vector<Value> args) override;
};
