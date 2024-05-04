// Header file declaring Regal tokens.

#ifndef TOKENS_H
#define TOKENS_H

// Token struct that assigns an integer and stores the integer value.
typedef struct {
    int num;
} Integ;

// Token struct that assigns an integer and stores the integer value.
typedef struct {
    char* name;
} Vari;

// Enum for all tokens in Regal.
typedef enum {
    // Keywords
    Let, Now,

    // Primitive data types
    Int,

    // Primitive Operators
    Plus, Minus, Mult, Div, Exp,

    // Variables
    Var,
    Bind,

    // Groupers
    LeftPar, RightPar,

    // Nothing token
    Nothing
} Token;

// Enum representing the number of each category of token above.
const static enum TokenTypeAmt {
    Keywrds = 2,
    Prims = 1,
    PrimOps = 5,
    Vars = 2,
    Groupers = 2,
    Nothings = 1
};

#endif