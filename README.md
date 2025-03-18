# Interpreter for simple programming language

My playground for tinkering and learning about programming languages.
Right now it is not very advanced but I plan to learn more about type systems
and turn it into something more robust.


# How to use this language

First you need to compile the interpreter

Requirements 

- CMake and c++ compiler that supports c++ 20

```
mkdir build && cd build
```

```
cmake ..
```

```
make
```

Now you can go into REPL by simply typing 

```
./main
```

from build directory, or pass name of a file with a program like so:

```
./main /path/to/file/program
```

---

Code wrote in this repo follows book ['Crafting interpreters'](https://craftinginterpreters.com/).


