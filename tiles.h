#ifndef TILES_H
#define TILES_H
#define TILE_GRID_SQR_DIM 5
#define TOTAL_ROTATION 4

#include "definition.h"

Tiles* initialise_tile_list(void);

void initialise_new_tile(Tiles*);

void free_tiles(Tiles*);

void rotate_tile(Tiles*);

void display_tiles(Tiles*, bool);

ErrorCode show_only_tiles(char*, Tiles*); 

bool validate_tile_dimension(int, int, int, int**);

ErrorCode validate_tile_file(char*, Tiles*);

void change_tiles(Game*);

#endif