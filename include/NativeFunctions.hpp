#pragma once

#include "Types.hpp"

#include <chrono>

namespace Native {
class Clock : public Callable {
public:
  Clock();
  Clock(const std::shared_ptr<Clock>& other);
  virtual std::string toString() override;
  virtual int arity() override;
  virtual Value call(Interpreter* interpreter, std::vector<Value> args) override;
};
}
