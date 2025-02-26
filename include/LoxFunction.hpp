#include "Types.hpp"
#include "Stmt.hpp"

class LoxFunction : public Callable {
  std::shared_ptr<Stmt::Function> declaration;
public:
  LoxFunction(std::shared_ptr<Stmt::Function> declaration);
  virtual int arity() override;
  virtual Value call(Interpreter* interpreter, std::vector<Value> args) override;
};
