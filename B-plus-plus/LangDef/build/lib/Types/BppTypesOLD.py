# Global variables representing token types in B++.
# Token types as defined in ./BppTypesREADME.txt
# Running this file prints the current maximum number attributed to a token.

# Keywords
KEY_LET = 1 # 'let'
KEY_NOW = 2 # 'now'
KEYWRDS = [KEY_LET, KEY_NOW]

# Primitives
PRIM_INT = 3 # Integer
    # Paired with the integer value.
PRIM_BOOL = 4 # 'true|false'
    # Paired with the boolean value.
PRIMS = [PRIM_INT, PRIM_BOOL]

# Nothing
NOTHING = 5 # 'nothing'

# Variables
VAR = 6 # Variable/function name
    # Paired with the name as a string.

# Primitive Operators
PO_PLUS = 7 # '+'
PO_MINUS = 8 # '-'
PO_MULT = 9 # '*'
PO_DIV = 10 # '/'
PO_EXP = 11 # '**'
PRIM_OPS = [PO_PLUS, PO_MINUS, PO_MULT, PO_DIV, PO_EXP]

# Standard Operators
SO_EQ = 13 # '=='
    # Paired with a positive integer type number.
SO_NEQ = 14 # '!='
    # Paired with a positive integer type number.
STD_OPS = [SO_EQ, SO_NEQ]

# Binding Operators
BO_EQ = 15 # '='
    # Paired with a positive integer type number.
BO_INC = 16 # '+='
BIND_OPS = [BO_EQ, BO_INC]

# Groupers
L_PAR = 17 # '('
    # Paired with a positive integer group number.
R_PAR = 18 # ')'
    # Paired with a positive integer group number.
GROUP = 19


import re
import os

def find_max_number(file_path):
    """find the maximum number attributed to a global variable in this file.
    
    Use Regex to find all global variable values in this file and return the max.
    """
    contents = open(file_path, 'r').read()
    
    # Find all global variable values in the file.
    matches = re.findall('^([A-Za-z_][A-Za-z0-9_]*)\\s*=\\s*(\\d+)\\s*', contents, re.MULTILINE)

    # Extract numbers from the matches and find the maximum.
    numbers = [int(match[1]) for match in matches]
    max_number = max(numbers) if numbers else None
    return max_number

# Read from this file and print the maximum number attributed to aglobal variable.
if __name__ == "__main__":
    file_path = os.path.abspath(__file__)
    max_number = find_max_number(file_path)
    if (max_number != None):
        print('Maximum global: ' + str(max_number))
    else:
        print("No numbers found.")