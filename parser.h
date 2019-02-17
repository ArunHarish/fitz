#ifndef PARSER_H
#define PARSER_H

#include "definition.h"

void display_errors(ErrorCode);

bool parse_header(char*, Game*);

void save_game(Game*, char**);

ErrorCode check_final_dim(int, int);

bool check_col(int, int);

ErrorCode load_saved_game(char*, Game*);

bool correct_rotation(int*);

bool human_prompt(Game*, Player*, int*);

void algorithm_output(int*, Player*);

ErrorCode validate_dimension(char*, char*, Game*);

ErrorCode validate_tile_file(char*, Tiles*);

ErrorCode parse_arguments(int, char**);

#endif
