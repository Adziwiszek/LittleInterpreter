#include "../include/LoxInstance.hpp"

LoxInstance::LoxInstance(LoxClass* klass): klass { klass } {}

std::string LoxInstance::toString() {
  return klass->name + " instance";
}
