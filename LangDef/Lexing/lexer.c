#include <stdlib.h>
// #include <regex.h>
#include <stdio.h>
#include <string.h>
#include "tokens.h"


void **lex_string(char *str) {
    void **token_lst;
    int **token_ident;
    char curr_char;
    int i, lst_buf, lded_tok_buf, buf_count, ld_count;

    lst_buf = 12;
    token_lst = malloc(lst_buf * sizeof(void*)); 
    // check mem alloc
    lded_tok_buf = (lst_buf / 3) + 1;
    token_ident = malloc(lded_tok_buf * sizeof(int*));
    // check mem alloc

    i = 0;
    curr_char = str[i];

    while (curr_char && (curr_char != '\n')) {
        if (curr_char == 'l') {
            if ((str[i + 1] == 'e') && (str[i + 2] == 't')) {
                if (buf_count == lst_buf) {
                    void *temp_ptr;

                    temp_ptr = realloc(token_lst, lst_buf * 2 * sizeof(void*));
                    if (temp_ptr == NULL) {
                        perror("Memory allocation failed: lexer.c -> 'let' keyword memory reallocation.");
                        free(token_lst);
                        free(token_ident);
                        exit(EXIT_FAILURE);
                    }
                    token_lst = temp_ptr;
                    lst_buf *= 2;
                }
                int *int_ptr;

                int_ptr = (int*)token_lst[buf_count];
                *int_ptr = Let;
                buf_count += 1;
            }
        }
    }

    return NULL;
}


/*

Check for the first insstance of the 'let' keyword (followed by at least one 
whitespace character) in the provided string. Return a void pointer to the 'Let' 
enum variable if a match is found.

const int lex_keywrd_let(char *string) {
    regex_t regex;
    
    // Compile a regex_t object with the 'let' keyword.
    const int reg_err = regcomp(&regex, "let[[:space:]]", 0);

    // Check for Regex compilation errors.
    if (reg_err != 0) {
        char *err_msg, *full_err_msg;

        // Retrieve the size of the compilation error string.
        const size_t len = regerror(reg_err, &regex, NULL, 0);
        
        // Allocate a string to store the error message.
        err_msg = malloc((len + 1) * sizeof(char));
        // Handle a memory allocation error for error message allocation.
        if (err_msg == NULL) {
            perror("Memory allocation failed in Regex 'let' error message.");
            exit(EXIT_FAILURE);
        }
        // Set length of the rest of the error label.
        const static int err_label = 29;
        
        // Full retrieve the Regex error and store it in reg_err.
        regerror(reg_err, &regex, err_msg, len + 1);
        // Allocate a full error label message.
        full_err_msg = (malloc((len + err_label) * sizeof(char)));

        // Handle a memory allocation error.
        if (full_err_msg == NULL) {
            perror("Memory allocation failed in Regex 'let' full message.");
            exit(EXIT_FAILURE);
        }

        // Create the full error message and free the previous temporary string.
        sprintf(full_err_msg, "Regex compilation failed: \"%s\".", err_msg);
        free(err_msg);

        // Print and free the error message then exit the program.
        perror(full_err_msg);
        free(full_err_msg);
        exit(EXIT_FAILURE);
    }
    regmatch_t *match;

    // Only matching for the first 'let' keyword in the string.
    const static int match_amt = 1;

    // Check for a match in the string then free compiled regex_t object.
    const int let_match = regexec(&regex, string, match_amt, match, REG_NOTEOL);
    regfree(&regex);

    // Check for a match.
    if (let_match == 0) {
        // decide how to return the values.
        const int end_of_match = (int)(match[0].rm_eo);

        return end_of_match;
    
    // Handle error for running out of memory during match.
    } else if (let_match == REG_ESPACE) {
        char *err_msg;

        // Size of error message being displayed.
        const static int err_label = 53;

        // Allocate memory for an error message that includes the string that errored.
        err_msg = malloc((strlen(string) + err_label) * sizeof(char));
        // Handle a memory allocation error.
        if (err_msg == NULL) {
            perror("Memory allocation failed in matching 'let' Regex.");
            exit(EXIT_FAILURE);
        }

        // Create and print the full error message.
        sprintf(err_msg, "Matching 'let' Regex ran out of memory on string \"%s\".", string);
        perror(err_msg);

        // Free the message string and exit the program.
        free(err_msg);
        exit(EXIT_FAILURE);
    }
    const static int err_val = -1; //temp prolly
    return err_val;
}
*/

// temp
int main() {
    char* str;
    return 0;
}