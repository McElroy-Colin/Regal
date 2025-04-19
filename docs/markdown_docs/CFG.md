# Context-Free Grammar that dictates order of syntax in Regal

- At least one whitespace character must separate certain tokens.
  - Tokens that use special symbols (e.g. `+` or `**` or `=`) do not need whitespace separating them from other tokens.
- Groups of non-terminals are labeled in all caps and indented under their labels.
- Non-terminals follow the CamelCase writing style and are **bold**.
- Terminals are inline code. (e.g. `example`)
- Undefined terminals are inline code surrounded with carrots. (e.g. `<example>`)
- The <span style="color: #d2d2d2;"><span style="color: #d2d2d2;">|</span></span> symbol represents an OR branch.

### SCOPE ANALYSIS:

**File** &rarr; `\n` **CodeScope**

**CodeScope** &rarr; **InscopeOperation** **BlockIndicator**
            <span style="color: #d2d2d2;">|</span> **IfBlock** **BlockIndicator**

**BlockIndicator** &rarr; `\n<matched indent>` **CodeScope** <span style="color: #d2d2d2;">|</span> `\n<unmatched indent>`
 * `<matched indent>` is as indented or more indented than the current scope  
 * `<unmatched indent>` is less indented than the curent scope, indicating the end of a scope


### MULTI-LINE OPERATIONS:

**IfBlock** &rarr; `if` **Expression** `\n<new indent>` **CodeScope** **ElseBlock**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<span style="color: #d2d2d2;">|</span> `if` **Expression** `\n<new indent>` **CodeScope**  
 * `<new_indent>` is a greater indent than the current scope, indicating a nested scope

**ElseBlock** &rarr; `\n<exact indent>` `else` **IfBlock**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<span style="color: #d2d2d2;">|</span> `\n<exact indent>` `else` `\n<new indent>` **CodeScope**  
 * `<exact indent>` is precisely the same indentation as the current scope  
 * `<new_indent>` is a greater indend than the current scope, indicating a nested scope


### SINGLE-LINE INITIALIZER:

**InscopeOperation** &rarr; **Assignment**


### VARIABLE ASSIGNMENT:

**Assignment** &rarr; **ExplicitAssignment** <span style="color: #d2d2d2;">|</span> **ImplicitAssignment**

**ExplicitAssignment** &rarr; `let` `<var>` `=` **Expression**
 * `<var>` is a custom variable name

**ImplicitAssignment** &rarr; `<var>` `=` **Expression**
 * `<var>` is an already defined variable name

<br>
Almost all tokens can be separated with any number of lines in an expression. These newlines will not affect scope.

### EXPRESSIONS:

**Expression** &rarr; **TernaryIfExpr**

**TernaryIfExpr** &rarr; **EquativeExpr** **`NO`**`\n` `if` **Expression** `else` **Expression** <span style="color: #d2d2d2;">|</span> **EquativeExpr**
 * **`NO`**`\n` means there cannot be a newline between the EquativeExpr and the `if`

### BOOLEAN ARITHMETIC:

**EquativeExpr** &rarr; **OrExpr** `<comp_op>` **EquativeExpr** <span style="color: #d2d2d2;">|</span> **OrExpr**
 * `<comp_op>` is `==` or `is`

**OrExpr** &rarr; **ExclusiveOrExpr** `<or_op>` **OrExpr** <span style="color: #d2d2d2;">|</span> **ExclusiveOrExpr**
 * `<or_op>` is `|` or `or`

**ExclusiveOrExpr** &rarr; **AndExpr** `<xor_op>` **ExclusiveOrExpr** <span style="color: #d2d2d2;">|</span> **AndExpr**
 * `<xor_op>` is `||` or `xor`

**AndExpr** &rarr; **NotExpr** `<and_op>` **AndExpr** <span style="color: #d2d2d2;">|</span> **NotExpr**
 * `<and_op>` is `&`or `and`

**NotExpr** &rarr; `<not_op>` **NotExpr** <span style="color: #d2d2d2;">|</span> **NumericalCompExpr**
 * `<not_op>` is `!` or `not`
    
**NumericalCompExpr** &rarr; **AdditiveExpr** `<numeric_comp_op>` **NumericalCompExpr** <span style="color: #d2d2d2;">|</span> **AdditiveExpr**
 * `<numeric_comp_op>` is `>`, `<`, `<=`, or `>=`


### NUMERICAL ARITHMETIC:

**AdditiveExpr** &rarr; **MultiplicativeExpr** `<add_op>` **AdditiveExpr** <span style="color: #d2d2d2;">|</span> **MultiplicativeExpr**
 * `<add_op>` is `+` or `-`

**MultiplicativeExpr** &rarr; **ExponentialExpr** `<mult_op>` **MultiplicativeExpr** <span style="color: #d2d2d2;">|</span> **ExponentialExpr**
 * `<mult_op>` is `*` or `/`

**ExponentialExpr** &rarr; **MinusIdentifierExpr** `**` **ExponentialExpr** <span style="color: #d2d2d2;">|</span> **MinusIdentifierExpr**


### LOW-LEVEL VALUES:

**MinusIdentifierExpr** &rarr; **PrimitiveExpr** <span style="color: #d2d2d2;">|</span> `-`**PrimitiveExpr**

**PrimitiveExpr** &rarr; `<var>` <span style="color: #d2d2d2;">|</span> **NumberExpr** <span style="color: #d2d2d2;">|</span> **BooleanExpr** <span style="color: #d2d2d2;">|</span> (**Expression**)
 * `<var>` is a custom variable name

**NumberExpr** &rarr; `<integer>`
 * `<integer>` is any integer

**BooleanExpr** &rarr; `true` <span style="color: #d2d2d2;">|</span> `false`
