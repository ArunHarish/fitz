#ifndef GAME_H
#define GAME_H

#include "definition.h"
#include "player.h"
#include "grid.h"
#include "tiles.h"
#include "token.h"
#include "parser.h"

int start_game(int, char**);

Game* initialise_game(void);

void free_game(Game*);

void place_move(int*, Player*, Game*, Tiles*);

void change_turn(Game*);

bool valid_move(int*, Player*, Board*, Tiles*);

void update_position_algorithm_one(int*, int, int*, int);

void algorithm_one(Game*, Player**); 

void update_position_algorithm_two(int*, int, int*, int, bool);

void algorithm_two(Game*, Player*);

bool check_possible_move(Game*, Player*);

ErrorCode game_loop(Game* game);

#endif