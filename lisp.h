#ifndef LISP_H
#define LISP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef enum {
    ATOM,
    CONS,
    NIL,
    FUNCTION
} NodeType;

typedef enum {
    INTEGER,
    FLOAT,
    SYMBOL,
    STRING
} AtomType;

typedef struct {
    AtomType type;
    union {
        long integer;
        double float_num;
        char *symbol;
        char *string;
    } value;
} Atom;

typedef struct {
    struct sexp *car;
    struct sexp *cdr;
} ConsCell;

typedef struct {
    struct sexp *params;
    struct sexp *body;
} Function;

typedef struct sexp {
    NodeType type;
    union {
        Atom atom;
        ConsCell conscell;
        Function function;
    } value;
} SExp;

typedef struct {
    char* input;
    int pos;
} Parser;

typedef SExp Environment;

typedef enum {
    FUN_UNKNOWN,
    FUN_QUOTE,
    FUN_SET,
    FUN_ADD,
    FUN_SUB,
    FUN_MUL,
    FUN_DIV,
    FUN_MOD,
    FUN_LT,
    FUN_GT,
    FUN_LTE,
    FUN_GTE,
    FUN_EQ,
    FUN_NOT,
    FUN_AND,
    FUN_OR,
    FUN_IF,
    FUN_COND,
    FUN_CONS,
    FUN_CAR,
    FUN_CDR,
    FUN_CADR,
    FUN_CDDR,
    FUN_CADDR,
    FUN_CDDDR,
    FUN_NIL_CHECK,
    FUN_ATOM_CHECK,
    FUN_SYMBOL_CHECK,
    FUN_NUMBER_CHECK,
    FUN_STRING_CHECK,
    FUN_LIST_CHECK,
    FUN_DEFINE,
    FUN_LAMBDA,
    FUN_LOAD,
    FUN_QUIT
} FunctionType;

// Function declarations
SExp* cons(SExp* car, SExp* cdr);
SExp* car(SExp* list);
SExp* cdr(SExp* list);
SExp* cadr(SExp* list);
SExp* cddr(SExp* list);
SExp* caddr(SExp* list);
SExp* cdddr(SExp* list);
SExp* createInteger(long n);
SExp* createFloat(double n);
SExp* createSymbol(const char *symbol);
SExp* createString(const char *string);
SExp* createNil(void);
SExp* createTruthy(void);
SExp* createError(const char *error_msg);
int isError(SExp* sexp);
void printSexp(SExp* sexp);
void printAtom(SExp* sexp);
void printCons(SExp* sexp);
void passWhitespace(Parser* parser);
void skipComment(Parser* parser);
char getNextNonWhitespaceChar(Parser* parser);
char getCurrentChar(Parser* parser);
void advance(Parser* parser);
SExp* parseNumber(Parser* parser);
SExp* parseSymbol(Parser* parser);
SExp* parseString(Parser* parser);
SExp* parseSexp(Parser* parser);
SExp* parseList(Parser* parser);
char* readFile(const char* filename);
int isValidNumber(Parser* parser);
int isValidSymbol(Parser* parser);
int next(Parser* parser);
SExp* nextSexp(Parser* parser);
SExp* isNilType(SExp* sexp);
SExp* isAtomType(SExp* sexp);
SExp* isSymbolType(SExp* sexp);
SExp* isNumberType(SExp* sexp);
SExp* isStringType(SExp* sexp);
SExp* isListType(SExp* sexp);
SExp* sexp_to_bool(SExp* sexp);
int isIntOrFloat(SExp* sexp);
double intToFloat(SExp* sexp);
SExp* eq_cons(SExp* a, SExp* b);
SExp* add(SExp* a, SExp* b);
SExp* sub(SExp* a, SExp* b);
SExp* mul(SExp* a, SExp* b);
SExp* div_op(SExp* a, SExp* b);
SExp* mod(SExp* a, SExp* b);
SExp* lt(SExp* a, SExp* b);
SExp* gt(SExp* a, SExp* b);
SExp* lte(SExp* a, SExp* b);
SExp* gte(SExp* a, SExp* b);
SExp* eq(SExp* a, SExp* b);
SExp* not_op(SExp* sexp);
Environment* createEnv(void);
SExp* set(Environment** env, SExp* symbol, SExp* value);
SExp* lookup(Environment* env, SExp* symbol);
FunctionType getFunctionType(SExp* symbol);
SExp* eval(Environment** env, SExp* sexp);
SExp* createFunction(SExp* params, SExp* body);

extern SExp* NIL_NODE;
extern SExp* TRUTHY_NODE;
extern Environment* GLOBAL_ENV;

#endif