# Regal V0.5

Compiler source code in progress.

Regal is a work-in-progress non-functional programming language developed by Colin McElroy. Currently, the language is unfinished with progress on compiler source code in the ~/src/src_langdef and ~/include directories. Regal is intended to have variable declaration (check!), function declaration, static type-checking (check!), implicit typing (check!), optimized evaluation (check!), and more. Updates will be posted in the main branch as progress continues. The dev branch will have unfinished updates posted as well.

The Regal playground (~/playground/playground.cpp) can be executed to generate an executable. This executable generates a simple text environment in the terminal to write and execute Regal code. Currently, Windows has full support, with support for other operating systems in progress.

Note: C++20 or later is required for execution of any compiler code.

V0.5 features:
 - Full boolean support, including arithmetic using '&'/'and', '|'/'or', '||'/'xor', '!'/'not', '>', '<', and '='.
 - Full static type-checking when assigning variables and evaluating arithmetic.
 - Impicit reassignment, 'let x = 7' initializes 'x', then 'x = 8' implicitly reassigns 'x' to a new value of the same type.
 - Tenary inline if-else statements, 'let x = 5 if y < 9 else y'.
 - Robust error support and debug upgrades.
 - Playground bug fixes. Reorganization of source code.
