#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

// needs to be finished then commented

// returns void** eventually
void lex_string(char *string) { // include -lregex in gcc compiling
    regex_t regex;

    const int reg_err = regcomp(&regex, "let", 0);

    if (reg_err == 0) {
        char *err_msg, *full_err_msg;

        const size_t len = regerror(reg_err, &regex, NULL, 0);

        err_msg = malloc((len + 1) * sizeof(char));

        if (err_msg == NULL) {
            perror("Memory allocation failed in Regex 'let' error message.");
            exit(EXIT_FAILURE);
        }

        const static int err_label = 29;
        
        regerror(reg_err, &regex, err_msg, len + 1);
        full_err_msg = (malloc((len + err_label) * sizeof(char)));

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
    regmatch_t *match;
    const static int match_amt = 1;

    const int let_match = regexec(&regex, string, match_amt, match, 0);

    if (let_match == REG_ESPACE) {
        char *err_msg;

        const static int err_label = 53;

        err_msg = malloc((strlen(string) + err_label) * sizeof(char));
        if (err_msg == NULL) {
            perror("Memory allocation failed in matching 'let' Regex.");
            exit(EXIT_FAILURE);
        }

        sprintf(err_msg, "Matching 'let' Regex ran out of memory on string \"%s\".", string);
        perror(err_msg);

        free(err_msg);
        exit(EXIT_FAILURE);
        
    } else if (let_match != 0) {
        
        // handle matched let

    }

    regfree(&regex);
}

// temp
int main() {
    char* str = "  let s";
    lex_string(str);
    return 0;
}