#include "../include/NativeFunctions.hpp"

Native::Clock::Clock(const std::shared_ptr<Clock>& other) {}

Native::Clock::Clock() { }

std::string Native::Clock::toString() { return "() -> Number"; }

int Native::Clock::arity() { return 0; }

Value Native::Clock::call(Interpreter* interpreter, std::vector<Value> args) {
  return std::chrono::duration<float>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}
