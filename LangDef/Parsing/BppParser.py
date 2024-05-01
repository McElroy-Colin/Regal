"""
File containing relevant functions for parsing a list of syntax tokens into an evaluation tree.

All functions follow directly from the Context-Free Grammar located in ./BppCFG.txt
"""

import sys
sys.path.append('../..')
from LangDef.Types.BppTypes import *
from LangDef.Parsing.ParseHelpers import *

def parse_line(tok_lst):
    """Parse a line of code and return a tree containing the order of evaluation.
    
    A line ends with the newline character or EOF. Lines are the
    highest expression in the B++ CFG.
    """
    # Return only the evaluation tree, hence the 1 index.
    return parse_keywrd_expr(tok_lst)[1]

def parse_keywrd_expr(tok_lst):
    """Parse a line beginning with a keyword and return an evaluation tree."""
    if (check(tok_lst, Let, 0)):
        return parse_let_expr(tok_lst)
    elif (check(tok_lst, Now, 0)):
        return parse_now_expr(tok_lst)
    else:
        raise ValueError("Line must start with a keyword.")
    
def parse_let_expr(tok_lst):
    """Parse a Let Expression as defined in the B++ CFG."""
    tok_lst = match_bypass(tok_lst, Let)[0]
    (tok_lst, var_tok) = match_bypass(tok_lst, Var)
    tok_lst = match_bypass(tok_lst, Bind)[0]
    (tok_lst, expr1) = parse_expr(tok_lst)
    # # LetVar contains a Var as the variable's name and the variables value an expression.
    return (tok_lst, LetVar((var_tok, expr1)))

def parse_now_expr(tok_lst):
    """Parse a Now Expression as defined in the B++ CFG."""
    tok_lst = match_bypass(tok_lst, Now)[0]
    (tok_lst, var_tok) = match_bypass(tok_lst, Var)
    tok_lst = match_bypass(tok_lst, Bind)[0]
    (tok_lst, expr1) = parse_expr(tok_lst)
    # NowVar contains a Var as the variable's name and the variables value an expression.
    return (tok_lst, NowVar((var_tok, expr1)))

def parse_expr(tok_lst):
    """Parse an Expression as defined in the B++ CFG."""
    if (check_many(tok_lst, KEYWRDS, 0)):
        return parse_keywrd_expr(tok_lst)
    else:
        return parse_base_expr(tok_lst)
    
def parse_base_expr(tok_lst):
    """Parse a Base Expression as defined in the B++ CFG."""
    if (check_many(tok_lst, [Int, LeftParen], 0)):
        return parse_math_expr(tok_lst)
    elif (check(tok_lst, Var, 0)):
        return parse_var_expr(tok_lst)
    else:
        raise ValueError("Base Expression must have a variable or Math Expression.")

def parse_math_expr(tok_lst):
    """Parse a Math Expression as defined in the B++ CFG."""
    return parse_add_expr(tok_lst)

# The order of the CFG/parsing operators inversely determines the precedence of evaluation without parenthesis.

def parse_add_expr(tok_lst):
    """Parse an Additive Expression as defined in the B++ CFG."""
    (tok_lst, mult_expr1) = parse_mult_expr(tok_lst)
    if (check_many(tok_lst, ADD_OPS, 0)):
        (tok_lst, add_op) = match_bypass_any(tok_lst, ADD_OPS)
        (tok_lst, add_expr1) = parse_add_expr(tok_lst)
        # Update the operator's default values to expressions.
        return (tok_lst, add_op.update((mult_expr1, add_expr1)))
    else:
        return (tok_lst, mult_expr1)

def parse_mult_expr(tok_lst):
    """Parse a Multiplicative Expression as defined in the B++ CFG."""
    (tok_lst, expon_expr1) = parse_expon_expr(tok_lst)
    if (check_many(tok_lst, MULT_OPS, 0)):
        (tok_lst, mult_op) = match_bypass_any(tok_lst, MULT_OPS)
        (tok_lst, mult_expr1) = parse_mult_expr(tok_lst)
        # Update the operator's default values to expressions.
        return (tok_lst, mult_op.update((expon_expr1, mult_expr1)))
    else:
        return (tok_lst, expon_expr1)
    
def parse_expon_expr(tok_lst):
    """Parse an Exponential Expression as defined in the B++ CFG."""
    (tok_lst, int_expr1) = parse_prim_expr(tok_lst)
    if (check(tok_lst, Exp, 0)):
        tok_lst = match_bypass(tok_lst, Exp)[0]
        (tok_lst, expon_expr1) = parse_expon_expr(tok_lst)
        return (tok_lst, Exp((int_expr1, expon_expr1)))
    else:
        return (tok_lst, int_expr1)
    
def parse_prim_expr(tok_lst):
    """Parse a Primary Expression as defined in the B++ CFG."""
    if (check(tok_lst, LeftParen, 0)):
        tok_lst = match_bypass(tok_lst, LeftParen)[0]
        (tok_lst, expr1) = parse_expr(tok_lst)
        tok_lst = match_bypass(tok_lst, RightParen)[0]
        return (tok_lst, expr1)
    elif (check_many(tok_lst, NUMS, 0)):
        return parse_num_expr(tok_lst)
    elif (check(tok_lst, Var, 0)):
        return parse_var_expr(tok_lst)
    else:
        raise ValueError("Primary Expression must have a number, variable, or Expression.")
    
def parse_num_expr(tok_lst):
    """Parse a Numerical Expression as defined in the B++ CFG."""
    return match_bypass_any(tok_lst, NUMS)

def parse_var_expr(tok_lst):
    """Parse a Variable Expression as defined in the B++ CFG."""
    return match_bypass(tok_lst, Var)