#ifndef GRID_H
#define GRID_H

#include "definition.h"


void grid_display(Board*);

void reallocate_grid(Board*, int, int);

bool valid_grid_content(int, int, char, Game*);

Board* initialise_grid(void);

void free_grid(Board*);

#endif