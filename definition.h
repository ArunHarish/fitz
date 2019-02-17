#ifndef FITZ_H
#define FITZ_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

/* Enum to store all possible error and their respective exit codes */
typedef enum {
    NO_ERROR = 0,
    WRONG_PARAM = 1,
    TILE_NOT_FOUND = 2,
    TILE_INVALID = 3,
    PLAYER_INVALID = 4,
    DIM_INVALID = 5,
    FILE_INACCESS = 6,
    FILE_INVALID = 7,
    EOF_DETECTED = 10
} ErrorCode;


typedef enum {
    SAVE_FILE,
    NO_TOKEN_ERROR,
    INVALID_INPUT
} TokenCode;

/* Enum type to describe the player type that is to be used in the main game */
typedef enum {
    HUMAN,
    P1_AUTO,
    P2_AUTO
} PlayerType;

/* 
    Player object that contains its symbol, PlayerType defined above,
    recent move made by this player, and describes whether the player is
    first player, which is necessary in the Type 2 algorithm. 
*/
typedef struct {
    char* symbol;
    PlayerType type;
    int recentMove[2];
    bool firstPlayer;
} Player;

/*  
    An object that stores the row - height, and column of the board, and stores
    information about each placement pointing to the Player object, which is
    described as 3D array.

    When drawing the symbol of the player is used, and if there is no placement
    in a particular coordinate NULL is used as an indication.
*/
typedef struct {
    int width;
    int height;
    Player*** grid;
} Board;

/*
    Tiles object that loads the tile file and contains information about total 
    tiles, the current tile index in use, and the Tile grid, which contains 
    information about the layout of possible placements within the board. This
    layout is a 4D array.
*/
typedef struct {
    int total;
    int current;
    int rotationIndex;
    int**** grid;
} Tiles;

/*
    Game object that contains information about two players, the board,
    contains the tile loaded, the current turn within the players and
    recent move made by either player used in Type 1 algorithm.
*/
typedef struct {
    Player* player1;
    Player* player2;
    Board* gameBoard;
    Tiles* tiles;
    int turn;
    int recentMove[2];
} Game;

/*
    Indicates the four rotations that are possible by the player. It is used
    for rotation array definition
*/
#define POSSIBLE_ROTATION 4

#endif