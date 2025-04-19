# Regal syntax
 - Tokens with duplicate syntax are separated with / *(e.g. `&`* / *`and`)*
 - All Unicode whitespace characters besides the newline character are ignored between tokens.
 - Most tokens are separated by whitespace. No token can contain whitespace characters.
 - All tokens are case-sensitive. There are no capital letters in keywords.

### Declarative tokens: 
 - *Variable/function labels*: *Variable/function labels* must start with a letter or `_` followed by any number of alphanumeric characters or `_`.
 - `let`: Denotes the assignment of a new variable. *(e.g. `let x = 3`)*
 - `=`: Assigns the given *variable/function label* to the given expression.

### Value tokens:
 - *Numbers*: Signed whole or decimal numbers. Negative numbers cannot have whitespace between the `-` and number characters. *(e.g. `-3.0`)*
 - `true`: The boolean value **True**.
 - `false`: The boolean value **False**.

### Operator tokens:
 - `if`: Keyword for **if-else** blocks or ternary operations.
 - `else`: Keyword in **if-else** blocks or ternary operations.
 - `+`: Adds two numbers.
 - `-`: Subtracts two numbers.
 - `*`: Multiplies two numbers.
 - `/`: Divides two numbers and truncates any decimal digits.
 - `**`: Two asterisks denote an exponential relationship between numbers. *(e.g. `2**3` is 2 to the 3rd power)*
 - `|` / `or`: Logical **OR** operation for two boolean values.
 - `||` / `xor`: Logical **XOR** operation for two boolean values.
 - `&` / `and`: Logical **AND** operation for two boolean values.
 - `!` / `not`: Logical **NOT** operation for a boolean value.
 - `>`: Compares two values, returning `true` if the left value is *larger* than the right value.
 - `<`: Compares two values, returning `true` if the left value is *smaller* than the right value.
 - `==` / `is`: Compares two values for equality.
 - `<=`: Compares two values, returning `true` if the left value is *smaller* or *equal* to the right value.
 - `>=`: Compares two values, returning `true` if the left value is *larger* or *equal* to the right value.

### Special tokens:
 - `#`: Denotes a **single-line comment**.
 - `##`: Denotes a **multi-line comment**. A **multi-line comment** closes with another `##`.
 - `(`: Denotes expression groupings to assign an order of operations to expressions.
 - `)`: Closes expression groupings.
 - `\n`: The newline character separates operations.