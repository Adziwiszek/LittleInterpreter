#include "../include/LoxClass.hpp"

LoxClass::LoxClass(const std::string& name) : name{name} {}

std::string LoxClass::toString() { return name; }
