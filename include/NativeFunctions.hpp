#pragma once

#include "Types.hpp"

#include <chrono>

namespace Native {
  class Clock : public Callable {
    virtual int arity() override;
    virtual Value call(Interpreter* interpreter, std::vector<Value> args) override;
  };
}
