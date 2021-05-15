# C-Compiler

The specification for this project is available [here](c_compiler.md)


## Overview

The following repository contains a working compiler which converts C to MIPS1 assembly. The source language is pre-processed C90. The target environment is Ubuntu 16.04, so the lab Ubuntu distribution, or equivalently an Ubuntu 16.04 VM as configured in the attached Vagrantfile. 

It supports the compilation of the following features:

* a file containing just a single function with no arguments
* variables of `int` type
* local variables
* arithmetic and logical expressions
* if-then-else statements
* while loops

* files containing multiple functions that call each other
* functions that take up to four parameters
* for loops
* arrays declared globally (i.e. outside of any function in your file)
* arrays declared locally (i.e. inside a function)
* reading and writing elements of an array
* recursive function calls
* the `enum` keyword
* `switch` statements
* the `break` and `continue` keywords

* variables of `double`, `float`, `char`, `unsigned`, structs, and pointer types
* calling externally-defined functions (i.e. the file being compiled declares a function, but its definition is provided in a different file that is linked in later on)
* functions that take more than four parameters
* mutually recursive function calls
* locally scoped variable declarations (e.g. a variable that is declared inside the body of a while loop, such as `while(...) { int x = ...; ... }`.
* the `typedef` keyword
* the `sizeof(...)` function (which takes either a type or a variable)
* taking the address of a variable using the `&` operator
* dereferencing a pointer-variable using the `*` operator
* pointer arithmetic
* character literals, including escape sequences like `\n`
* strings (as NULL-terminated character arrays)
* declaration and use of structs

## Guide for Use

The compiler will be built by running the following command in the top-level directory of your repo:

    make bin/c_compiler

The compilation function is invoked using the flag `-S`, with the source file and output file specified on the command line:

    bin/c_compiler -S [source-file.c] -o [dest-file.s]
