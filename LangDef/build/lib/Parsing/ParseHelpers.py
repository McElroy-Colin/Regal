"""
File containing relevant helper functions for parsing a list of syntax tokens into an evaluation tree.
"""

import sys
sys.path.append('..')
from Types.BppTypes import *

def check(tok_lst, token, index):
    """Check if token is the element of tok_lst at index.

    Return a boolean conveying above information. Error if index is out of range. 
    Ignore data stored in the element of tok_lst. (i.e. Int(12) and Int(-4) are the same)
    """
    length = len(tok_lst)
    if (length == 0):
        return False
    elif (length <= index):
        raise IndexError("Parsing for " + str(token) + ", index out of range.")
    return (isinstance(tok_lst[index], token))

def check_many(tok_lst, tokens, index):
    """Check if any of the elements of tokens is the element of tok_lst at index.

    Return True if any one of the tokens is the same class as tok_lst 
    at index. Error if index is out of range.
    """
    length = len(tok_lst)
    if (length == 0):
        return False
    elif (length <= index):
        raise IndexError("Parsing for " + str(tokens) + ", index out of range.")
    
    # Extract the current token to compare with.
    query_tok = tok_lst[index]

    # Compare the current token to each element of tokens.
    for token in tokens:
        if (isinstance(query_tok, token)):
            return True
    return False

def check_token_many(token, targ_lst):
    for i in range(len(targ_lst)):
        if (isinstance(token, targ_lst[i])):
            return True
    return False

def match_bypass(tok_lst, token):
    """Return tok_lst without the first element if that element is an instance of token.
    
    Check that token is the class at index 0 of tok_lst. Error if tok_lst is empty 
    or its element does not match token. Return the rest of tok_lst after the first element 
    paired with any value that may be associated with the matched element.
    """
    if (len(tok_lst) == 0):
        raise IndexError("Matching for " + str(token) + " on empty list.")
    
    # Extract the first element from tok_lst.
    curr_tok = tok_lst[0]

    # Compare the element to token.
    if (isinstance(curr_tok, token)):
        # Return the rest of tok_lst after this element as well as that token's associated value.
        return (tok_lst[1:], curr_tok)
    else:
        raise ValueError("Tried to match " + str(token) + " but had " + str(curr_tok) + ".")
    
def match_bypass_any(tok_lst, tokens):
    """Return tok_lst without the first element if that element 
    matches any of tokens.
    
    Check that some element of tokens is the same class as index 0 of tok_lst. 
    Error if tok_lst is empty or element does not match any of tokens. 
    Return the rest of tok_lst after the first element paired with 
    any value that may be associated with the matched element.
    """
    if (len(tok_lst) == 0):
        raise IndexError("Matching for any of " + str(tokens) + " on empty list.")

    # Extract the first token from tok_lst.
    query_tok = tok_lst[0]

    # Compare tok_lst's element to each element of tokens.
    for token in tokens:
        if (isinstance(query_tok, token)):
             # Return the rest of tok_lst alongside the matched token.
             return (tok_lst[1:], query_tok)   
    raise ValueError("Tried to match " + str(query_tok) + " on any of " + str(tokens))

def match_bypass_all(tok_lst, tokens):
    """Match successive elements of tok_lst to successive elements of 
    tokens starting from index 0 of both.
    
    Check that successive elements of tokens and tok_lst are the same class, starting 
    from index 0 of both. Error if tok_lst is too small or if any match fails. 
    Return the remainder of tok_lst after all of tokens has been matched.
    """
    match_amt = len(tokens)
    if (len(tok_lst) < match_amt):
        raise IndexError("Matching for all of " + str(tokens) + " on too small a list.")
    
    # Iterate through tok_lst and tokens, comparing each element of matching index. 
    for i in range(match_amt):
        query_tok = tok_lst[i]
        curr_match = tokens[i]

        # Error if any corresponding elements of tok_lst and tokens do not match.
        if (not isinstance(query_tok, curr_match)):
            raise ValueError("Tried to match " + str(query_tok) + " but had " + str(curr_match) + ".")
    return tok_lst[(i + 1):]

def group_to_prim(op_group):
    """Return the evaluator equivalent of the group primitive operator given."""
    if (isinstance(op_group, AddG)):
        return Add
    elif (isinstance(op_group, SubG)):
        return Sub
    elif (isinstance(op_group, MultG)):
        return Mult
    elif (isinstance(op_group, DivG)):
        return Div
    elif (isinstance(op_group, ExpG)):
        return Exp