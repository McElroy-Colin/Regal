"""
File to individually test strings for output using LangDef features.
"""

import sys
sys.path.append('../..')
from LangDef.Lexing.BppLexer import lex_string
from LangDef.Parsing.BppParser import parse_line

TEST_STRING = "let y = let x = 3 ** 4 - z + (2 * 4) - 4"

BOOL_TEST = False # Test against expected results instead of just printing.

# Only fill in if BOOL_TEST is True
TEST_AGAINST_LEX = True
EXPECTED_LEX = []

TEST_AGAINST_PARSE = True
EXPECTED_PARSE = None

TEXT_AGAINST_EVAL = False # Unimplemented so far.
EXPECTED_EVAL = 0 # Unimplemented so far.
# End of BOOL_TEST

def test_str(string, tst_lex, tst_parse, tst_eval):
    """Compare given stages of compilation of a string with expected values."""
    token_list = lex_string(string)
    eval_tree = parse_line(token_list)
    # evaluate here
    if (tst_eval):
        return True
        # compare eval to expected eval
    elif (tst_parse):
        return (eval_tree == EXPECTED_PARSE)
    elif (tst_lex):
        return (token_list == EXPECTED_LEX)
    return None

if (BOOL_TEST):
    outcome = test_str(TEST_STRING, TEST_AGAINST_LEX, TEST_AGAINST_PARSE, TEXT_AGAINST_EVAL)
    print(outcome)
    
# Print results instead.
else:
    token_list = lex_string(TEST_STRING)
    print(token_list)
    eval_tree = parse_line(token_list)
    print(eval_tree)
    # eval printing here