# MiniC-Compiler

## Description

This repository contains code for a full-fledged Front-End for the MiniC language, more details regarding the language can be found in *LanguageManual.pdf*. The project was completed in the following phases:

### Parsing phase

The micro-syntax and macro-syntax can be found in *MiniC.g4*. The tokenizing and parsing is being automatically done using Antlr4-runtime and toolchanin.

### AST generation and Semantic Analysis phase

The code for building AST can be found in *Ast.h* and *BuildAstVisitor.h*. Visitor design pattern has been used to facilitate clean and efficient production. Semantic Analysis code can be found in *SemanticAnalysisVisitor.h* and Symbol Table class in *SymbolTable.h*. The checks being done are:

* Type Checking
* Variable Declaration

### LLVM IR generation phase

*IRGenVisitor.h* contains code for generating non-optimized LLVM IR for the code, which can be further executed using LLVM tools like the lli interpreter.

A decent amount of sample programs are available in *Sample Programs* directory which can be used for extensive testing and demonstration.

## How To Run

```bash
make
```

Running *make* will compile the entire code and generate some Antlr files and two executables *main* and *bonus*.

```bash
./main <path to .mc file>
```

This will generate LLVM IR for the code in a file called *temp.ll* in your main directory. This is in human readable format.

```bash
llvm-as temp.ll
lli temp.bc
```

This will convert the IR to bitcode format which is then ready to be executed by the LLVM Interpreter lli.

```bash
./bonus
```

This will automatically read the *temp.bc* file and print the Extended Basic Blocks from the Control Flow Graph which are used in Super Value Numbering.

```bash
make clean
```

Use this to clean up the generated files.

## Author

* Vaibhav Garg
