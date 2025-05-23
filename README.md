# Interpreter for simple programming language

A simple interpreter for a toy programming language - this is my playground to explore programming languages.
I want to learn about lexing, parsing, type systems.

Inspired by [*Crafting Interpreters*](https://craftinginterpreters.com/)

---

## Features and future plans

- ~~Classes~~ *Done*
- ~~Resolver~~ *Done*
- Simple static type system *In progress*
- Improve REPL *Planned*
- Inheritance *Planned*
- Rewrite lexer and parser in style of lex and yacc *Planned*
- Generic types *Planned*

---

## Requirements

- C++20 compatible compiler (e.g. `g++`, `clang++`)
- [CMake](https://cmake.org/) ≥ 3.16

---

## Build & Run

### Step 1 – Build

```bash
git clone https://github.com/twoj-nick/interpreter.git
cd interpreter
mkdir build && cd build
cmake ..
make
```

### Step 2 - Run

#### REPL

```bash
./main
```

#### Run a program from a file

```bash
./main /path/to/program
```

---

## Example program

```
fun fib(n) {
    if(n <= 1) {
        return n;
    }
    return fib(n-1) + fib(n-2);
}

print fib(5);
```

---

## Project structure (TODO)

