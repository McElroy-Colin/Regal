B++ tokens and their meta types:

Type descriptions are formatted as such:
    1) Case by case substitutable descriptions are surrounded by '<>'.
    2) Unknown types are written as 't1', 't2', and so on.
    3) The '(s)' qualifier denotes one or more of that type.
    4) Multiple inputs are separated by a ', '.
    5) The '->' qualifier denotes the output type of operators.



 '\n' : The newline character has no type.


Keywords : 'keyword'

    Words that cannot be used as variable/function names and have a specific use case in B++.

        'let' : 'keyword'
        'now' : 'keyword'


Primitives : 'primitive <prim_type>'

    Tokens that evaluate to an expression of the same type. Below are the Primitive types.

        Integer : 'primitive int'
        'true' : 'primitive bool'
        'false' : 'primitive bool'

Nothing : 'nothing'

    The Nothing token evaluates to type 'nothing'.

        'nothing' : 'nothing'
    

Variables : 'var'

    Unassigned variable/function names are given the 'var' type.

        Variable/function name : 'var'


Primitive Operators : '<prim_type>(s) -> <prim_type> ; prim_op'

    Operators that take any number of Primitives and output a Primitive.

        '+' : 'primitive int, primitive int -> primitive int ; prim_op'
        '-' : 'primitive int, primitive int -> primitive int ; prim_op'
        '*' : 'primitive int, primitive int -> primitive int ; prim_op'
        '/' : 'primitive int, primitive int -> primitive int ; prim_op'
        '**' : 'primitive int, primitive int -> primitive int ; prim_op'


Standard Operators : '<t1>, <t2> -> <prim_type> ; std_op'

    Operators that take two expressions of unknown type and output a Primitive.

        '==' : 't1, t1 -> primitive bool ; std_op'
        '!=' : 't1, t1 -> primitive bool ; std_op'


Binding Operators : '<t1>(s) -> <prim_type> ; bind_op'

    Operators that take one or more expressions of varying type and output Nothing. These generally perform some non-output related action.

        '=' : 'var, t1 -> nothing'
        '+=' : 'primitive int, primitive int -> nothing'


Groupers: '<left|right> grouper <<group_num>>'

    Refers to parenthesis that group expressions to set precedence. The parenthesis can be either 'left' or 'right' and will be given a group number. Group numbers start at 1 on the outermost parenthesis of an expression and increase as parenthesis are nested.

        '(' : 'left grouper <group_num>'
        ')' : 'right grouper <group_num>'