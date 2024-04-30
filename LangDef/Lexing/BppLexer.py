"""
File containing relevant function for lexing a string into syntax tokens for B++.
"""

import re

import sys
sys.path.append('../..')
from LangDef.Types.BppTypes import *


def lex_string(input):
    """Extract relevant B++ tokens and their types from a string.
    
    Partition the provided string into tokens following the lexemes B++ accepts as syntax. 
    Return a list of the tokens as defined in './Types/BppTypes.py' ordered according to how they 
    appeared in the string. Pair specific tokens with a value or variable type number.
    """
    if (type(input) != str):
        raise ValueError("Input must be a string.")
    token_lst = [] # List or types to return
    type_count = 1 # Keep track of unknown types
    group_count = 1 # Keep track of parenthesis
    inp_pos = 0 # Current index in the original string
    str_pos = 0 # Temporary index in the lexed string.
    curr_str = input

    # Loop until the string has been fully tokenized. Use str_pos to continue 
    # tokenizing past already lexed parts of the string.
    while (str_pos < len(curr_str)):
        curr_str = curr_str[str_pos:]

        # Check for a token from the current index of the string onwards. Use
        # Regex to check for multicharacter tokens. Update token_lst and set 
        # str_pos past the identified token for the next iteration.

        # Bypass Unicode whitespace characters besides newline.
        if ((matched := re.match('[ \\t\\r\\f\\v]+', curr_str)) != None):
            
            # Set the current position to past the identified whitespace.
            str_pos = len(matched.group())

            # Update the total position past the whitespace.
            inp_pos += str_pos

        # Check for negative integers encased in parenthesis.
        elif ((matched := re.match('\\(-[0-9]+\\)', curr_str)) != None):
            curr_token = (matched.group()[1:])[:-1] # Remove wrapping parenthesis.
            token_lst.append(Int(int(curr_token)))
            str_pos = len(curr_token) + 2 # Add 2 for parenthesis.
            inp_pos += str_pos

        # Check for regular integers.
        elif ((matched := re.match('[0-9]+', curr_str)) != None):
            curr_token = matched.group()
            token_lst.append(Int(int(curr_token)))
            str_pos = len(curr_token)
            inp_pos += str_pos

        # Check for '**' symbol.
        elif (re.match('\\*\\*', curr_str) != None):
            token_lst.append(Exp((None, None))) # Operator values default to None
            str_pos = 2
            inp_pos += str_pos

        # Check for '==' symbol.
        elif (re.match('==', curr_str) != None):
            
            # add the '==' symbol alongside a number corresponding to the 
            # relative types of each argument.
            token_lst.append(IsEqual(type_count))

            # Increment the type counter to distinguish the next labeled tokens 
            # as a different type than these.
            type_count += 1
            str_pos = 2
            inp_pos += str_pos

        # Check for '+=' symbol.
        elif (re.match('\\+=', curr_str) != None):
            token_lst.append(Increm())
            str_pos = 2
            inp_pos += str_pos

        # Check for '!=' symbol.
        elif (re.match('!=', curr_str) != None):
            token_lst.append(NotEqual(type_count))
            type_count += 1
            str_pos = 2
            inp_pos += str_pos

        # Check for 'let' keyword.
        elif (re.match('\\blet\\b', curr_str) != None):
            token_lst.append(Let())
            str_pos = 3
            inp_pos += str_pos

        # Check for 'now' keyword.
        elif (re.match('\\bnow\\b', curr_str) != None):
            token_lst.append(Now())
            str_pos = 3
            inp_pos += str_pos

        # Check for 'nothing' keyword.
        elif (re.match('\\bnothing\\b', curr_str) != None):
            token_lst.append(Nothing())
            str_pos = 7
            inp_pos += str_pos

        # Check for boolean keywords.
        elif ((matched := re.match('true|false', curr_str)) != None):
            curr_token = True
            str_pos = 0
            if (matched.group()[0] == 'f'):
                curr_token = False
                str_pos = 1
            token_lst.append(Bool(bool(curr_token)))
            str_pos += 4
            inp_pos += str_pos

        # Check for custom variable/function names.
        elif ((matched := re.match('[a-zA-Z][a-zA-Z0-9_]*', curr_str)) != None):
            curr_token = matched.group()
            token_lst.append(Var(curr_token))
            str_pos = len(curr_token)
            inp_pos += str_pos

        # Check for single character lexemes.

        # Stop lexing on a newline character.
        elif (curr_str[0] == '\n'):
            break
        
        # Lex the '=' character.
        elif (curr_str[0] == '='):
            token_lst.append(Bind(type_count))
            type_count += 1
            str_pos = 1
            inp_pos += str_pos

        # Lex the '+' character.
        elif (curr_str[0] == '+'):
            token_lst.append(Add((None, None))) # Operator values default to None
            str_pos = 1
            inp_pos += str_pos

        # Lex the '-' character.
        elif (curr_str[0] == '-'):
            token_lst.append(Sub((None, None))) # Operator values default to None
            str_pos = 1
            inp_pos += str_pos

        # Lex the '*' character.
        elif (curr_str[0] == '*'):
            token_lst.append(Mult((None, None))) # Operator values default to None
            str_pos = 1
            inp_pos += str_pos

        # Lex the '/' character.
        elif (curr_str[0] == '/'):
            token_lst.append(Div((None, None))) # Operator values default to None
            str_pos = 1
            inp_pos += str_pos

        # Lex the '(' character.
        elif (curr_str[0] == '('):
            token_lst.append(LeftParen(group_count))
            group_count += 1
            str_pos = 1
            inp_pos += str_pos

        # Lex the ')' character.
        elif (curr_str[0] == ')'):
            group_count -= 1
            token_lst.append(RightParen(group_count))
            str_pos = 1
            inp_pos += str_pos

        # Error block if current token is not in B++ syntax.
        else:
            
            # Isolate current line.
            if ((matched := re.search('\\n', input)) != None):
                # Set input to be only the current line.
                input = input[:-(len(input) - matched.end() + 1)]

                # Surround the error token in brackets.
                if ((matched := re.search('\\s', curr_str)) != None):
                    end_pos = inp_pos + matched.end() - 1
                    lhalf = input[0:inp_pos]
                    rhalf = input[end_pos:]
                    problem_tok = input[inp_pos:end_pos]
                    raise ValueError('(Lexing Error)\n' + lhalf + '{' + problem_tok + '}' + rhalf + '\nInvalid token bracketed above.')
            
            # Still error in case of Regex issues.

                raise ValueError('(Lexing Error)\n' + input + '\nInvalid token.')
            raise ValueError('(Lexing Error)\n' + curr_str + '\nFirst token invalid.')
    
    return token_lst