#include "lisp.h"

///////////////////////////////////////
// START OF STRUCTS AND CONSTRUCTORS //
///////////////////////////////////////

SExp* NIL_NODE;
SExp* TRUTHY_NODE;

// Create and return new conscell
SExp* cons(SExp* car, SExp* cdr) {
    SExp* node = malloc(sizeof(SExp));
    node->type = CONS;
    node->value.conscell.car = car;
    node->value.conscell.cdr = cdr;
    return node;
}

// Return first element of conscell
SExp* car(SExp* list) {
    if (list->type == CONS) {
        return list->value.conscell.car;
    }
    return createNil();
}

// Return the rest of a conscell's lists
SExp* cdr(SExp* list) {
    if (list->type == CONS) {
        return list->value.conscell.cdr;
    }
    return createNil();
}

SExp* cadr(SExp* list) {
    return car(cdr(list));
}

SExp* cddr(SExp* list) {
    return cdr(cdr(list));
}

SExp* caddr(SExp* list) {
    return car(cddr(list));
}

SExp* cdddr(SExp* list) {
    return cdr(cddr(list));
}

SExp* createInteger(long n) {
    SExp* node = malloc(sizeof(SExp));
    node->type = ATOM;
    node->value.atom.type = INTEGER;
    node->value.atom.value.integer = n;
    return node;
}

SExp* createFloat(double n) {
    SExp* node = malloc(sizeof(SExp));
    node->type = ATOM;
    node->value.atom.type = FLOAT;
    node->value.atom.value.float_num = n;
    return node;
}

SExp* createSymbol(const char *symbol) {
    SExp* node = malloc(sizeof(SExp));
    node->type = ATOM;
    node->value.atom.type = SYMBOL;
    node->value.atom.value.symbol = strdup(symbol);
    return node;
}

SExp* createString(const char *string) {
    SExp* node = malloc(sizeof(SExp));
    node->type = ATOM;
    node->value.atom.type = STRING;
    node->value.atom.value.string = strdup(string);
    return node;
}

SExp* createNil() {
    if (!NIL_NODE) {
        NIL_NODE = malloc(sizeof(SExp));
        NIL_NODE->type = NIL;
    }
    return NIL_NODE;
}

SExp* createTruthy() {
    if (!TRUTHY_NODE) {
        TRUTHY_NODE = malloc(sizeof(SExp));
        TRUTHY_NODE->type = ATOM;
        TRUTHY_NODE->value.atom.type = SYMBOL;
        TRUTHY_NODE->value.atom.value.symbol = strdup("'t");
    }
    return TRUTHY_NODE;
}

SExp* createError(const char *error_msg) {
    SExp* node = malloc(sizeof(SExp));
    node->type = ATOM;
    node->value.atom.type = SYMBOL;
    node->value.atom.value.symbol = strdup(error_msg);
    return node;
}

SExp* createFunction(SExp* params, SExp* body) {
    SExp* node = malloc(sizeof(SExp));
    node->type = FUNCTION;
    node->value.function.params = params;
    node->value.function.body = body;
    return node;
}

/////////////////////////////////////
// END OF STRUCTS AND CONSTRUCTORS //
/////////////////////////////////////

///////////////////////
// START OF PRINTING //
///////////////////////

void printSexp(SExp* sexp) {
    if (!sexp) {
        printf("Error");
        return;
    }
    switch (sexp->type) {
        case ATOM:
            printAtom(sexp);
            break;
        case CONS:
            printCons(sexp);
            break;
        case NIL:
            printf("()");
            break;
        case FUNCTION:
            printf("function ");
            printSexp(sexp->value.function.params);
            printf(" ");
            printSexp(sexp->value.function.body);
    }
}

void printAtom(SExp* sexp) {
    Atom* atom = &(sexp->value.atom);
    switch (atom->type) {
        case INTEGER:
            printf("%ld", atom->value.integer);
            break;
        case FLOAT:
            printf("%f", atom->value.float_num);
            break;
        case SYMBOL:
            printf("%s", atom->value.symbol);
            break;
        case STRING:
            printf("\"%s\"", atom->value.string);
            break;
    }
}

void printCons(SExp* sexp) {
    printf("(");
    
    // Flag to determine if it is the first element or not
    int first = 1;
    
    while (sexp->type == CONS) {
        if (!first) {
            printf(" "); // Add space before non-first elements
        }
        first = 0;
        
        printSexp(sexp->value.conscell.car);
        SExp* rest = sexp->value.conscell.cdr;
        
        switch (rest->type) {
            case NIL:
                printf(")");
                return;
            case CONS:
                sexp = rest;
                break;
            default:
                printf(" . ");
                printSexp(rest);
                printf(")");
                return;
        }
    }
}

/////////////////////
// END OF PRINTING //
/////////////////////

//////////////////////
// START OF PARSING //
//////////////////////

void passWhitespace(Parser* parser) {
    while (parser->pos < strlen(parser->input)) {
        char c = parser->input[parser->pos];
        if (isspace((unsigned char)c)) {
            parser->pos++;
        } else if (c == ';') {
            skipComment(parser);
        } else {
            break;
        }
    }
}

void skipComment(Parser* parser) {
    char c = getCurrentChar(parser);
    if (c == ';') {
        while (c != '\0' && c != '\n') {
            advance(parser);
            c = getCurrentChar(parser);
        }
        if (c == '\n') {
            advance(parser);
        }
    }
}

char getNextNonWhitespaceChar(Parser* parser) {
    passWhitespace(parser);
    if (parser->pos >= strlen(parser->input)) return '\0';
    return parser->input[parser->pos];
}

char getCurrentChar(Parser* parser) {
    if (parser->pos >= strlen(parser->input)) return '\0';
    return parser->input[parser->pos];
}

void advance(Parser* parser) {
    if (parser->pos < strlen(parser->input)) {
        parser->pos++;
    }
}

SExp* parseNumber(Parser* parser) {
    char buffer[256];
    int i = 0;
    int has_decimal = 0;
    char c = getCurrentChar(parser);
    
    // Handle sign
    if (c == '-' || c == '+') {
        buffer[i++] = c;
        advance(parser);
        c = getCurrentChar(parser);
    }
    
    // Parse digits and optional decimal point
    while (c != '\0' && (isdigit(c) || c == '.') && !isspace(c) && c != '(' && c != ')' && c != '"' && i < 255) {
        if (c == '.') {
            has_decimal = 1;
        }
        buffer[i++] = c;
        advance(parser);
        c = getCurrentChar(parser);
    }
    
    buffer[i] = '\0';
    
    if (has_decimal) {
        double value = atof(buffer);
        return createFloat(value);
    } else {
        long value = atol(buffer);
        return createInteger(value);
    }
}

SExp* parseSymbol(Parser* parser) {
    char buffer[256];
    int i = 0;
    char c = getCurrentChar(parser);
    
    while (c != '\0' && c != '(' && c != ')' && !isspace(c) && i < 255) {
        buffer[i++] = c;
        advance(parser);
        c = getCurrentChar(parser);
    }
    
    buffer[i] = '\0';
    return createSymbol(buffer);
}

SExp* parseString(Parser* parser) {
    char buffer[1024];
    int i = 0;
    
    // Skip opening quote
    advance(parser);
    
    char c = getCurrentChar(parser);
    while (c != '\0' && c != '"' && i < 1023) {
        buffer[i++] = c;
        advance(parser);
        c = getCurrentChar(parser);
    }
    
    if (c == '"') { // Valid string
        advance(parser);
        buffer[i] = '\0';
        return createString(buffer);
    } else { // Only 1 quote, handle as symbol
        char symbol_buffer[1024];
        symbol_buffer[0] = '"';
        strcpy(symbol_buffer + 1, buffer);
        return createSymbol(symbol_buffer);
    }
}

SExp* parseList(Parser* parser) {
    // Skip opening parenthesis
    advance(parser);
    
    char c = getNextNonWhitespaceChar(parser);
    
    // Nil
    if (c == ')') {
        advance(parser);
        return createNil();
    }
    
    if (c == '\0') {
        return createError("ERROR_UNCLOSED_PARENTHESIS");
    }

    // Parse first element
    SExp* head = parseSexp(parser);
    if (isError(head)) return head;
    
    // Parse remaining elements
    SExp* current = cons(head, createNil());
    SExp* result = current;
    
    while (1) {
        c = getNextNonWhitespaceChar(parser);
        
        if (c == ')') {
            advance(parser);
            break;
        }
        
        SExp* next_elem = parseSexp(parser);
        if (isError(next_elem)) return next_elem;
        
        SExp* new_cons = cons(next_elem, createNil());
        current->value.conscell.cdr = new_cons;
        current = new_cons;
    }
    return result;
}

char* readFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(size+1);
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';
    
    fclose(file);
    return buffer;
}

int isValidNumber(Parser* parser) {
    int saved_pos = parser->pos;
    int has_decimal = 0;
    int has_digits = 0;
    
    char c = getCurrentChar(parser);
    
    // Handle sign
    if (c == '-' || c == '+') {
        parser->pos++;
        c = getCurrentChar(parser);
    }
    
    // Check digits and decimal point
    while (c != '\0' && !isspace(c) && c != '(' && c != ')') {
        if (isdigit(c)) {
            has_digits = 1;
        } else if (c == '.') {
            if (has_decimal) {
                // Multiple decimal points, not a number
                parser->pos = saved_pos;
                return 0;
            }
            has_decimal = 1;
        } else {
            // Non-digit, not a number
            parser->pos = saved_pos;
            return 0;
        }
        parser->pos++;
        c = getCurrentChar(parser);
    }
    
    parser->pos = saved_pos;
    return has_digits;
}

int isValidSymbol(Parser* parser) {
    int saved_pos = parser->pos;
    char c = getCurrentChar(parser);
    
    while (c != '\0' && !isspace((unsigned char)c) && c != '(' && c != ')') {
        if (0) { // Rework this with banned characters
            return 0;
        }
        parser->pos++;
        c = getCurrentChar(parser);
    }
    
    parser->pos = saved_pos;
    return 1;
}

int isError(SExp* sexp) {
    if (!sexp) return 1;
    if (sexp->type != ATOM) return 0;
    if (sexp->value.atom.type != SYMBOL) return 0;
    
    char* symbol = sexp->value.atom.value.symbol;
    return (strncmp(symbol, "ERROR_", 6) == 0);
}

SExp* parseSexp(Parser* parser) {
    char c = getNextNonWhitespaceChar(parser);
    if (c == '\0') {
        return createError("ERROR_UNCLOSED_PARENTHESIS");
    }

    if (c == '(') {
        return parseList(parser);
    }

    if (c == ')') {
        advance(parser);
        return createError("ERROR_UNEXPECTED_CLOSING_PARENTHESIS");
    }

    if (c == '\'') {
        advance(parser);
        SExp* quoted_expr = parseSexp(parser);
        if (isError(quoted_expr)) {
            return quoted_expr;
        }
        SExp* quote_symbol = createSymbol("QUOTE");
        SExp* quote_list = cons(quote_symbol, cons(quoted_expr, createNil()));
        return quote_list;
    }

    if (c == '"') {
        // Check if there is a closing "
        int saved_pos = parser->pos;
        advance(parser);
        char test_c = getCurrentChar(parser);
        while (test_c != '\0' && test_c != '"') {
            advance(parser);
            test_c = getCurrentChar(parser);
        }
        
        if (test_c == '"') { // Parse as string
            parser->pos = saved_pos;
            return parseString(parser);
        } else { // Parse as symbol
            parser->pos = saved_pos;
            return parseSymbol(parser);
        }
    }
    
    if (isValidNumber(parser)) {
        return parseNumber(parser);
    }
    
    if (isValidSymbol(parser)) {
        return parseSymbol(parser);
    }

    return createError("ERROR_GENERAL_INVALID_INPUT");
}

int next(Parser* parser) {
    passWhitespace(parser);
    return parser->pos < strlen(parser->input);
}

SExp* nextSexp(Parser* parser) {
    return parseSexp(parser);
}

////////////////////
// END OF PARSING //
////////////////////

////////////////////////////
// START OF TYPE CHECKERS //
////////////////////////////

SExp* isNilType(SExp* sexp) {
    if (sexp && sexp->type == NIL) {
        return createTruthy();
    }
    return createNil();
}

SExp* isAtomType(SExp* sexp) {
    if (!sexp || sexp->type == NIL || sexp->type == ATOM) {
        return createTruthy();
    }
    return createNil();
}

SExp* isSymbolType(SExp* sexp) {
    if (sexp && sexp->type == ATOM && sexp->value.atom.type == SYMBOL) {
        return createTruthy();
    }
    return createNil();
}

SExp* isNumberType(SExp* sexp) {
    if (sexp && sexp->type == ATOM && (sexp->value.atom.type == INTEGER || sexp->value.atom.type == FLOAT)) {
        return createTruthy();
    }
    return createNil();
}

SExp* isStringType(SExp* sexp) {
    if (sexp && sexp->type == ATOM && sexp->value.atom.type == STRING) {
        return createTruthy();
    }
    return createNil();
}

SExp* isListType(SExp* sexp) {
    if (sexp && (sexp->type == CONS || sexp->type == NIL)) {
        return createTruthy();
    }
    return createNil();
}

SExp* sexp_to_bool(SExp* sexp) {
    if (sexp && sexp->type != NIL) {
        return createTruthy();
    }
    return createNil();
}

//////////////////////////
// END OF TYPE CHECKERS //
//////////////////////////

////////////////////////////////////////
// START OF ARITHMETIC AND RELATIONAL //
////////////////////////////////////////

// Helpers start //
int isIntOrFloat(SExp* sexp) {
    return (sexp && sexp->type == ATOM && (sexp->value.atom.type == INTEGER || sexp->value.atom.type == FLOAT));
}

double intToFloat(SExp* sexp) {
    if (sexp->value.atom.type == INTEGER) {
        return (double)sexp->value.atom.value.integer;
    }
    // If a float is passed, just return it
    return sexp->value.atom.value.float_num;
}
SExp* eq_cons(SExp* a, SExp* b) {
    if (a->type != CONS || b->type != CONS) {
        return createNil();
    }
    SExp* compare_car = eq(car(a), car(b));
    if (!compare_car || compare_car->type == NIL) {
        return createNil();
    }
    return eq(cdr(a), cdr(b));
}
// Helpers end //

SExp* add(SExp* a, SExp* b) {
    if (!isIntOrFloat(a) || !isIntOrFloat(b)) {
        return createError("ERROR_TYPE_MISMATCH");
    }
    if ((a->value.atom.type == INTEGER) && (b->value.atom.type == INTEGER)) {
        return createInteger(a->value.atom.value.integer + b->value.atom.value.integer);
    }
    return createFloat(intToFloat(a) + intToFloat(b));
}

SExp* sub(SExp* a, SExp* b) {
    if (!isIntOrFloat(a) || !isIntOrFloat(b)) {
        return createError("ERROR_TYPE_MISMATCH");
    }
    if ((a->value.atom.type == INTEGER) && (b->value.atom.type == INTEGER)) {
        return createInteger(a->value.atom.value.integer - b->value.atom.value.integer);
    }
    return createFloat(intToFloat(a) - intToFloat(b));
}

SExp* mul(SExp* a, SExp* b) {
    if (!isIntOrFloat(a) || !isIntOrFloat(b)) {
        return createError("ERROR_TYPE_MISMATCH");
    }
    if ((a->value.atom.type == INTEGER) && (b->value.atom.type == INTEGER)) {
        return createInteger(a->value.atom.value.integer * b->value.atom.value.integer);
    }
    return createFloat(intToFloat(a) * intToFloat(b));
}

SExp* div_op(SExp* a, SExp* b) {
    if (!isIntOrFloat(a) || !isIntOrFloat(b)) {
        return createError("ERROR_TYPE_MISMATCH");
    }
    if (intToFloat(b) == 0.0) {
        return createError("ERROR_DIVISION_BY_ZERO");
    }
    return createFloat(intToFloat(a) / intToFloat(b));
}

SExp* mod(SExp* a, SExp* b) {
    if (!isIntOrFloat(a) || !isIntOrFloat(b)) {
        return createError("ERROR_TYPE_MISMATCH");
    }
    if (intToFloat(b) == 0.0) {
        return createError("ERROR_DIVISION_BY_ZERO");
    }
    if ((a->value.atom.type == INTEGER) && (b->value.atom.type == INTEGER)) {
        return createInteger(a->value.atom.value.integer % b->value.atom.value.integer);
    }
    return createFloat(fmod(intToFloat(a), intToFloat(b)));
}

SExp* lt(SExp* a, SExp* b) {
    if (!isIntOrFloat(a) || !isIntOrFloat(b)) {
        return createError("ERROR_TYPE_MISMATCH");
    }
    if (intToFloat(a) < intToFloat(b)) return createTruthy();
    return createNil();
}

SExp* gt(SExp* a, SExp* b) {
    if (!isIntOrFloat(a) || !isIntOrFloat(b)) {
        return createError("ERROR_TYPE_MISMATCH");
    }
    if (intToFloat(a) > intToFloat(b)) return createTruthy();
    return createNil();
}

SExp* lte(SExp* a, SExp* b) {
    if (!isIntOrFloat(a) || !isIntOrFloat(b)) {
        return createError("ERROR_TYPE_MISMATCH");
    }
    if (intToFloat(a) <= intToFloat(b)) return createTruthy();
    return createNil();
}

SExp* gte(SExp* a, SExp* b) {
    if (!isIntOrFloat(a) || !isIntOrFloat(b)) {
        return createError("ERROR_TYPE_MISMATCH");
    }
    if (intToFloat(a) >= intToFloat(b)) return createTruthy();
    return createNil();
}

SExp* eq(SExp* a, SExp* b) {
    // Both NIL
    if ((!a || a->type == NIL) && (!b || b->type == NIL)) {
        return createTruthy();
    }
    // Different NodeTypes
    if (!a || !b || a->type != b->type) {
        return createNil();
    }
    // Atom NodeType
    if (a->type == ATOM) {
        if (a->value.atom.type != b->value.atom.type) {
            if (isIntOrFloat(a) && isIntOrFloat(b)) {
                if (fabs(intToFloat(a) - intToFloat(b)) < 0.0001) return createTruthy();
            }
            return createNil();
        }
        if (a->value.atom.type == INTEGER) {
            return (a->value.atom.value.integer == b->value.atom.value.integer) ? createTruthy() : createNil();
        }
        if (a->value.atom.type == FLOAT) {
            return (fabs(a->value.atom.value.float_num - b->value.atom.value.float_num) < 0.0001) ? createTruthy() : createNil();
        }
        if (a->value.atom.type == SYMBOL) {
            return (strcmp(a->value.atom.value.symbol, b->value.atom.value.symbol) == 0) ? createTruthy() : createNil();
        }
        if (a->value.atom.type == STRING) {
            return (strcmp(a->value.atom.value.string, b->value.atom.value.string) == 0) ? createTruthy() : createNil();
        }
    }
    // Cons NodeType
    if (a->type == CONS) {
        return eq_cons(a, b);
    }
    return createNil();
}

SExp* not_op(SExp* sexp) {
    if (!sexp || sexp->type == NIL) return createTruthy();
    return createNil();
}

//////////////////////////////////////
// END OF ARITHMETIC AND RELATIONAL //
//////////////////////////////////////

///////////////////////////////////
// START OF ENVIRONMENT AND EVAL //
///////////////////////////////////

Environment* GLOBAL_ENV = NULL;

Environment* createEnv(void) {
    SExp* symbols = createNil();
    SExp* values = createNil();
    return cons(symbols, values);
}

SExp* set(Environment** env, SExp* symbol, SExp* value) {
    if (!env || !*env || !symbol || symbol->type != ATOM || symbol->value.atom.type != SYMBOL) {
        return createError("ERROR_INVALID_SET");
    }

    SExp* symbols = car(*env);
    SExp* values = cdr(*env);
    
    SExp* new_symbols = cons(symbol, symbols);
    SExp* new_values = cons(value, values);
    
    *env = cons(new_symbols, new_values);
    return value;
}

SExp* lookup(Environment* env, SExp* symbol) {
    if (!env || !symbol || symbol->type != ATOM || symbol->value.atom.type != SYMBOL) {
        return createError("ERROR_INVALID_LOOKUP");
    }
    
    SExp* symbols = car(env);
    SExp* values = cdr(env);
    
    while (symbols && symbols->type == CONS && values && values->type == CONS) {
        SExp* current_symbol = car(symbols);
        SExp* current_value = car(values);
        
        if (current_symbol && current_symbol->type == ATOM && current_symbol->value.atom.type == SYMBOL && strcmp(current_symbol->value.atom.value.symbol, symbol->value.atom.value.symbol) == 0) {
            return current_value;
        }
        
        symbols = cdr(symbols);
        values = cdr(values);
    }
    return createError("ERROR_UNDEFINED_SYMBOL");
}

FunctionType getFunctionType(SExp* symbol) {
    if (!symbol || symbol->type != ATOM || symbol->value.atom.type != SYMBOL) {
        return FUN_UNKNOWN;
    }
    
    char* name = symbol->value.atom.value.symbol;
    
    if (strcmp(name, "QUOTE") == 0 || strcmp(name, "quote") == 0) return FUN_QUOTE;
    if (strcmp(name, "SET") == 0 || strcmp(name, "set") == 0) return FUN_SET;
    
    if (strcmp(name, "ADD") == 0 || strcmp(name, "add") == 0 || strcmp(name, "+") == 0) return FUN_ADD;
    if (strcmp(name, "SUB") == 0 || strcmp(name, "sub") == 0 || strcmp(name, "-") == 0) return FUN_SUB;
    if (strcmp(name, "MUL") == 0 || strcmp(name, "mul") == 0 || strcmp(name, "*") == 0) return FUN_MUL;
    if (strcmp(name, "DIV") == 0 || strcmp(name, "div") == 0 || strcmp(name, "/") == 0) return FUN_DIV;
    if (strcmp(name, "MOD") == 0 || strcmp(name, "mod") == 0 || strcmp(name, "%") == 0) return FUN_MOD;
    
    if (strcmp(name, "LT") == 0 || strcmp(name, "lt") == 0 || strcmp(name, "<") == 0) return FUN_LT;
    if (strcmp(name, "GT") == 0 || strcmp(name, "gt") == 0 || strcmp(name, ">") == 0) return FUN_GT;
    if (strcmp(name, "LTE") == 0 || strcmp(name, "lte") == 0 || strcmp(name, "<=") == 0) return FUN_LTE;
    if (strcmp(name, "GTE") == 0 || strcmp(name, "gte") == 0 || strcmp(name, ">=") == 0) return FUN_GTE;
    if (strcmp(name, "EQ") == 0 || strcmp(name, "eq") == 0 || strcmp(name, "=") == 0) return FUN_EQ;
    if (strcmp(name, "NOT") == 0 || strcmp(name, "not") == 0 || strcmp(name, "!") == 0) return FUN_NOT;

    if (strcmp(name, "AND") == 0 || strcmp(name, "and") == 0 || strcmp(name, "&&") == 0) return FUN_AND;
    if (strcmp(name, "OR") == 0 || strcmp(name, "or") == 0 || strcmp(name, "||") == 0) return FUN_OR;
    if (strcmp(name, "IF") == 0 || strcmp(name, "if") == 0) return FUN_IF;
    if (strcmp(name, "COND") == 0 || strcmp(name, "cond") == 0) return FUN_COND;
    
    if (strcmp(name, "CONS") == 0 || strcmp(name, "cons") == 0) return FUN_CONS;
    if (strcmp(name, "CAR") == 0 || strcmp(name, "car") == 0) return FUN_CAR;
    if (strcmp(name, "CDR") == 0 || strcmp(name, "cdr") == 0) return FUN_CDR;
    if (strcmp(name, "CADR") == 0 || strcmp(name, "cadr") == 0) return FUN_CADR;
    if (strcmp(name, "CDDR") == 0 || strcmp(name, "cddr") == 0) return FUN_CDDR;
    if (strcmp(name, "CADDR") == 0 || strcmp(name, "caddr") == 0) return FUN_CADDR;
    if (strcmp(name, "CDDDR") == 0 || strcmp(name, "cdddr") == 0) return FUN_CDDDR;
    
    if (strcmp(name, "NIL?") == 0 || strcmp(name, "nil?") == 0) return FUN_NIL_CHECK;
    if (strcmp(name, "ATOM?") == 0 || strcmp(name, "atom?") == 0) return FUN_ATOM_CHECK;
    if (strcmp(name, "SYMBOL?") == 0 || strcmp(name, "symbol?") == 0) return FUN_SYMBOL_CHECK;
    if (strcmp(name, "NUMBER?") == 0 || strcmp(name, "number?") == 0) return FUN_NUMBER_CHECK;
    if (strcmp(name, "STRING?") == 0 || strcmp(name, "string?") == 0) return FUN_STRING_CHECK;
    if (strcmp(name, "LIST?") == 0 || strcmp(name, "list?") == 0) return FUN_LIST_CHECK;
    
    if (strcmp(name, "DEFINE") == 0 || strcmp(name, "define") == 0 || strcmp(name, "DEF") == 0 || strcmp(name, "def") == 0) return FUN_DEFINE;
    if (strcmp(name, "LAMBDA") == 0 || strcmp(name, "lambda") == 0) return FUN_LAMBDA;

    if (strcmp(name, "LOAD") == 0 || strcmp(name, "load") == 0) return FUN_LOAD;

    if (strcmp(name, "QUIT") == 0 || strcmp(name, "quit") == 0) return FUN_QUIT;

    return FUN_UNKNOWN;
}

SExp* eval(Environment** env, SExp* sexp) {
    if (!sexp || sexp->type == NIL) {
        return createNil();
    }
    
    if (sexp->type == ATOM) {
        if (sexp->value.atom.type == SYMBOL) {
            if (getFunctionType(sexp) != FUN_UNKNOWN) {
                return sexp;
            }
            SExp* result = lookup(*env, sexp);
            return result;
        }
        else {
            return sexp;
        }
    }
    
    if (sexp->type == CONS) {
        SExp* fun = car(sexp);

        if (!fun) {
            return createError("ERROR_EMPTY_FUNCTION_CALL");
        }
        
        FunctionType fun_type = getFunctionType(fun);
        
        switch (fun_type) {
            case FUN_QUOTE:
                return cadr(sexp);
            case FUN_SET: {
                SExp* symbol = cadr(sexp);
                SExp* value = eval(env, caddr(sexp));
                if (isError(value)) return value;
                return set(env, symbol, value);
            }
            case FUN_ADD: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return add(arg1, arg2);
            }
            case FUN_SUB: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return sub(arg1, arg2);
            }
            case FUN_MUL: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return mul(arg1, arg2);
            }
            case FUN_DIV: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return div_op(arg1, arg2);
            }
            case FUN_MOD: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return mod(arg1, arg2);
            }
            case FUN_LT: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return lt(arg1, arg2);
            }
            case FUN_GT: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return gt(arg1, arg2);
            }
            case FUN_LTE: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return lte(arg1, arg2);
            }
            case FUN_GTE: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return gte(arg1, arg2);
            }
            case FUN_EQ: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return eq(arg1, arg2);
            }
            case FUN_NOT: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return not_op(arg);
            }
            case FUN_AND: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                if (!arg1 || arg1->type == NIL) return createNil();
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return arg2;
            }
            case FUN_OR: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                if (arg1 && arg1->type != NIL) return createTruthy();
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return arg2;
            }
            case FUN_IF: {
                SExp* test = eval(env, cadr(sexp));
                if (isError(test)) return test;
                if (test && test->type != NIL) {
                    SExp* then_exp = eval(env, caddr(sexp));
                    if (isError(then_exp)) return then_exp;
                    return then_exp;
                }
                SExp* else_exp = eval(env, caddr(cdr(sexp)));
                if (isError(else_exp)) return else_exp;
                return else_exp;
            }
            case FUN_COND: {
                SExp* clauses = cdr(sexp);
                while (clauses && clauses->type == CONS) {
                    SExp* test_expr = car(clauses);
                    if (!test_expr) return createError("ERROR_INVALID_COND");

                    clauses = cdr(clauses);
                    if (!clauses || clauses->type != CONS) {
                        return createError("ERROR_MISSING_COND_RESULT");
                    }
                    
                    SExp* result_expr = car(clauses);
                    
                    SExp* test = eval(env, test_expr);
                    if (isError(test)) return test;
                    
                    if (test && test->type != NIL) {
                        SExp* result = eval(env, result_expr);
                        if (isError(result)) return result;
                        return result;
                    }
                    clauses = cdr(clauses);
                }
                return createNil();
            }
            case FUN_CONS: {
                SExp* arg1 = eval(env, cadr(sexp));
                if (isError(arg1)) return arg1;
                SExp* arg2 = eval(env, caddr(sexp));
                if (isError(arg2)) return arg2;
                return cons(arg1, arg2);
            }
            case FUN_CAR: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return car(arg);
            }
            case FUN_CDR: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return cdr(arg);
            }
            case FUN_CADR: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return cadr(arg);
            }
            case FUN_CDDR: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return cddr(arg);
            }
            case FUN_CADDR: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return caddr(arg);
            }
            case FUN_CDDDR: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return cdddr(arg);
            }
            case FUN_NIL_CHECK: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return isNilType(arg);
            }
            case FUN_ATOM_CHECK: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return isAtomType(arg);
            }
            case FUN_SYMBOL_CHECK: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return isSymbolType(arg);
            }
            case FUN_NUMBER_CHECK: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return isNumberType(arg);
            }
            case FUN_STRING_CHECK: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return isStringType(arg);
            }
            case FUN_LIST_CHECK: {
                SExp* arg = eval(env, cadr(sexp));
                if (isError(arg)) return arg;
                return isListType(arg);
            }
            case FUN_DEFINE: {
                SExp* name = cadr(sexp);
                SExp* second = caddr(sexp);
                
                if (!name || name->type != ATOM || name->value.atom.type != SYMBOL) {
                    return createError("ERROR_INVALID_FUNCTION_NAME");
                }
                
                if (second && second->type == CONS) {
                    SExp* first_elem = car(second);
                    if (first_elem && first_elem->type == ATOM && first_elem->value.atom.type == SYMBOL && (strcmp(first_elem->value.atom.value.symbol, "lambda") == 0 || strcmp(first_elem->value.atom.value.symbol, "LAMBDA") == 0)) {
                        SExp* lambda_result = eval(env, second);
                        if (isError(lambda_result)) return lambda_result;
                        return set(env, name, lambda_result);
                    } else {
                        SExp* params = second;
                        SExp* body = caddr(cdr(sexp));
                        SExp* fun = createFunction(params, body);
                        return set(env, name, fun);
                    }
                }
                
                SExp* value = eval(env, second);
                if (isError(value)) return value;
                return set(env, name, value);
            }
            case FUN_LAMBDA: {
                SExp* params = cadr(sexp);
                SExp* body = caddr(sexp);
                return createFunction(params, body);
            }
            case FUN_LOAD: {
                SExp* filename_exp = cadr(sexp);
                char* filename = NULL;
                if (filename_exp->type == ATOM) {
                    if (filename_exp->value.atom.type == STRING) {
                        filename = filename_exp->value.atom.value.string;
                    } else if (filename_exp->value.atom.type == SYMBOL) {
                        filename = filename_exp->value.atom.value.symbol;
                    }
                }
                
                if (!filename) {
                    return createError("ERROR_INVALID_FILENAME");
                }
                
                char* file_contents = readFile(filename);
                if (!file_contents) {
                    return createError("ERROR_FILE_NOT_FOUND");
                }
                
                Parser parser;
                parser.input = file_contents;
                parser.pos = 0;
                
                SExp* last_result = createNil();
                while (next(&parser)) {
                    SExp* parsed = nextSexp(&parser);
                    if (parsed) {
                        if (isError(parsed)) {
                            free(file_contents);
                            return parsed;
                        }
                        printf(">");
                        printSexp(parsed);
                        printf("\n");
                        last_result = eval(env, parsed);
                        if (isError(last_result)) {
                            free(file_contents);
                            return last_result;
                        }
                        printf(":");
                        printSexp(last_result);
                        printf("\n");
                    }
                }
                
                free(file_contents);
                printf(":");
                return createSymbol("SUCCESSFULLY_LOADED_FILE");
            }
            case FUN_QUIT: {
                printf("Thanks for using the interpreter!\n");
                exit(0);
            }
            case FUN_UNKNOWN:
            default:
                SExp* fun_to_call = NULL;
                
                if (fun->type == CONS) {
                    fun_to_call = eval(env, fun);
                    if (isError(fun_to_call)) return fun_to_call;
                }
                else if (fun->type == FUNCTION) {
                    fun_to_call = fun;
                }
                else if (fun->type == ATOM && fun->value.atom.type == SYMBOL) {
                    fun_to_call = lookup(*env, fun);
                    if (isError(fun_to_call)) return fun_to_call;
                }
                if (fun_to_call && fun_to_call->type == FUNCTION) {
                    SExp* params = fun_to_call->value.function.params;
                    SExp* body = fun_to_call->value.function.body;
                    
                    Environment* local_env = *env;
                    SExp* param_list = params;
                    SExp* arg_list = cdr(sexp);
                    
                    while (param_list && param_list->type == CONS) {
                        if (!arg_list || arg_list->type != CONS) {
                            return createError("ERROR_TOO_FEW_ARGUMENTS");
                        }
                        
                        SExp* param = car(param_list);
                        if (!param || param->type != ATOM || param->value.atom.type != SYMBOL) {
                            return createError("ERROR_INVALID_PARAMETER");
                        }
                        
                        SExp* arg_value = eval(env, car(arg_list));
                        if (isError(arg_value)) return arg_value;
                        
                        SExp* param_copy = createSymbol(param->value.atom.value.symbol);
                        set(&local_env, param_copy, arg_value);
                        
                        param_list = cdr(param_list);
                        arg_list = cdr(arg_list);
                    }
                    
                    if (arg_list && arg_list->type == CONS) {
                        return createError("ERROR_TOO_MANY_ARGUMENTS");
                    }
                    
                    return eval(&local_env, body);
                }
                return createError("ERROR_UNKNOWN_FUNCTION");
        }
    }
    return createError("ERROR_INVALID_EVAL");
}

/////////////////////////////////
// END OF ENVIRONMENT AND EVAL //
/////////////////////////////////
