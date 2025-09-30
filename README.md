
# Toy Compiler —  (C++)

**Project:** Compiler Backend for Managed Runtimes — Toy Compiler (Demo)  
**Author:** Ashish Kumar 
**Language:** C++
**Purpose:** Educational toy compiler that demonstrates lexical analysis, parsing (recursive descent),
AST construction, a simple optimization pass (constant folding), and generation of three-address code (TAC).

---

## Features
- Tokenizer (identifiers, integers, operators, parentheses, assignment, semicolon)
- Recursive descent parser for expressions and assignment statements
- AST with evaluation-friendly nodes
- Simple semantic checking (basic detection of malformed expressions)
- Optimization: constant folding on AST
- Back-end: Generates simple three-address code (TAC) with temporaries
- Makefile for easy build on Linux
- Examples directory with sample input programs

This repository is intentionally compact and well-documented to be easy to explain during interviews.

---

## Build & Run (Linux / WSL / macOS)
```bash
# build
make

# run with an example
./toy_compiler examples/example1.calc
```

The compiler prints the generated three-address code to stdout.

---

## Language supported (mini-language)
- Assignment statements: `x = 2 + 3 * (y + 1);`
- Each statement must end with a semicolon `;`
- Identifiers: letter followed by letters/digits/underscore (e.g., `sum1`)
- Integers: non-negative decimal integers
- Operators: `+ - * /` and parentheses

---

## Example
Input (examples/example1.calc):
```
a = 2 + 3 * (4 + 5);
b = a + 7;
c = (b + 2) * 3;
```

Output (TAC):
```
t1 = 4 + 5
t2 = 3 * t1
t3 = 2 + t2
a = t3
t4 = a + 7
b = t4
t5 = b + 2
t6 = t5 * 3
c = t6
```

---

## Files of interest
- `src/main.cpp` — full compiler driver (tokenizer, parser, AST, optimizer, codegen)
- `Makefile` — build instructions
- `examples/example1.calc` — sample input file
- `README.md` — this document

---

## Notes for interview
- This project is intentionally modular: you can expand the parser to support conditionals, loops, functions, or integrate a register allocator to map TAC to x86/ARM.
- The optimization pass is illustrative; you can discuss implementing further passes such as dead-code elimination, copy propagation, common subexpression elimination, and loop-invariant code motion.
- The code uses modern C++ (C++11+) and is designed to compile with `g++`.

---

If you want, I can:
- push this to a GitHub repo and provide the link,
- add unit tests, or
- expand back-end to emit assembly (x86-64).
