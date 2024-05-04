// Header file declaring TEMP<Elixir> tokens.

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

// Enum for all tokens in TEMP<Elixir>.
typedef enum {
    // Keywords
    Let,
    Now,

    // Primitive data types
    Int,

    // Primitive Operators
    Plus,
    Minus,
    Mult,
    Div,
    Exp,

    // Variables
    Var,
    Bind,

    // Groupers
    LeftPar,
    RightPar,

    // Nothing token
    Nothing
} Token;

#endif