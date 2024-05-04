#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

// needs to be finished then commented
void lex_string(char* string) { // include -lregex in gcc compiling
    regex_t letexpr;

    const int let_ret = regcomp(&letexpr, "\\blet\\b", 0);

    if (let_ret != 0) {
        size_t len;
        char* err_msg, full_err_msg;

        len = regerror(let_ret, &letexpr, NULL, 0);
        err_msg = malloc((len + 1) * sizeof(char));

        if (err_msg == NULL) {
            perror("Memory allocation failed in Regex 'let' error message.");
            exit(EXIT_FAILURE);
        }

        const static int err_label = 29;
        
        regerror(let_ret, &letexpr, err_msg, len + 1);
        full_err_msg = malloc((len + err_label) * sizeof(char));

        if (full_err_msg == NULL) {
            perror("Memory allocation failed in Regex 'let' full message.");
            exit(EXIT_FAILURE);
        }

        sprintf(full_err_msg, "Regex compilation failed: \"%s\".", err_msg);
        free(err_msg);

        perror(full_err_msg);
        free(full_err_msg);
        exit(EXIT_FAILURE);
    }

    // actually match let_expr here

    regfree(&letexpr);
}