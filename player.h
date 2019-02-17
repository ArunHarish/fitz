#ifndef PLAYER_H
#define PLAYER_H
#include "definition.h"

ErrorCode validate_player_type(char*, char*, Game*);

Player* initialise_player(bool);

void free_players(Game*);

#endif