Trenton Ottman
LISP Interpreter Written in C

-- README --
This project is an implementation of a lisp interpreter written in C. Right now, it is in a limited functioning state.

Currently, the program has the capability to act as a REPL allowing the user to write and evaluate lisp code. 
Some key features of the program are the ability to read s-expressions (Atoms, Cons, or NIL) from the command line and from file, print s-expressions, 
perform basic type checks, handle list manipulation, perform arithmetic and logical functions, evaluate s-expressions, create user defined functions,
create lambda functions, perform short circuit functions (and + or), conditional statements (if and cond), and load s-expressions from file. 

The organization of the code is as follows:
    Main - lispinterpreter.c
    Header - lisp.h
    Function Implementation - functions.c
    Standard Library Functions - stdlib.lisp
    
I use lispinterpreter.c to execute the REPL. This file is effectively just a while loop to read stdin.
To keep my code clean, I use lisp.h to define all of my functions.
Every function listed in lisp.h is implemented in functions.c.

To build the main interpreter I use:
    gcc -Wall lispinterpreter.c functions.c
    ./a.exe or ./a.out

Near the top of lisp.h you can view the basic data structures used.
The basic overview is as follows:
    SExp - struct consisting of a type (ATOM, CONS, or NIL) and a value (Atom or ConsCell)
    Atom - struct consisting of a type (INTEGER, FLOAT, SYMBOL, STRING) and a value (integer, float_num, symbol, string)
    ConsCell - struct consting of 2 sexp (car and cdr)
    Function - struct consisting of 2 sexp (params and body)
    Parser - struct used to help parse stdin
    Environment - s-expression containing a list of 2 lists (symbols and values)

Limitations:
    There is no way to write an escaped characters
    There is no way to write a multiline s-expressin in the REPL, it must be done from file and loaded
    Modulo is not working properly for a specific edge case
    ' as the only input throws the wrong error message
    No garbage collection
    Issues that I have not yet discovered
    
Choices:
    I added the ability to create negative and postive numbers by prefixing + or - on a number
    Strings without a closing quote are handled as symbols
    Comments can be added with the ';' character
    Files may be loaded with (load "filename") or (load filename)
    I wrote a few useful functions that I call my standard library, can be loaded with (load stdlib.lisp)
    The interpreter can be exited with (QUIT) or (quit)

