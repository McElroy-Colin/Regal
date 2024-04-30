Accepted syntax in B++


All Unicode whitespace characters besides the newline character are ignored between tokens.

Most tokens are separated by whitespace. No token can contain whitespace characters.

All tokens are case-sensitive. There are not capital letters in keywords.



Definition tokens:

    Variable/function names: Variable/function names are non-case-sensitive but must start with 
    a letter followed by any number of alphanumeric characters as well as '_'.

    'let': Denotes the assignment of a new variable or function. (i.e. 'let x = 3')

    'now': Denotes the reassigment of an existing variable or function. (i.e. 'now x = 4')

    '=': Assigns the given variable/function name to the given expression.

    '+=': Increments an integer variable by the specified amount. (i.e. 'x += 5' is 'x = x + 5')


Boolean tokens:

    'true': The boolean value True. (NOT a keyword)

    'false': The boolean value False. (NOT a keyword)

    '==': Compares two expressions for equality.

    '!=': Compares to expressions for non-equality.


Integer tokens:

    Integers: Whole numbers of any length. Negative numbers are denoted with '(-<int>)' to differentiate 
    from subtraction. (i.e. '(-23)' is the integer -23)

    '**': Two asterisks denote an exponential relationship between integers. (i.e. '2**3' is 2 to the 3rd power)

    '+': Adds two integers.

    '-': Subtracts two integers.

    '*': Multiplies two integers.

    '/': Divides two integters and truncates any decimal digits.

    '(': Denotes math groupings to assign an order of opoerations to equations.

    ')': Closes math groupings.


Special tokens:

    'nothing': The equivalent of 'NULL', 'nil', or '()' in other languages.

    '\n': The newline character separates expressions.