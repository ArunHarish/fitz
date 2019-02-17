#ifndef TOKEN_H
#define TOKEN_H
#define MAX_USER_INP 80
#define DEFAULT_TOKEN_SIZE 10

#include "definition.h"

char** create_tokens(int, int);

TokenCode validate_tokens(char** tokens, int* command);

void reallocate_token(char**, int, int);

void free_tokens(char**, int);

void tokenise_string(char**, char*, int);

bool tokenise_input(char**, int);

TokenCode convert_token_to_int(char**, int*, int);

#endif