#include "player.h"

/* 
    Validates player argument is of right type and sets it to the game object 

    @param player1 the first player input given by the user as argument
    @param player2 the second player input given by the user as argument
    @param game the object that contains information about grid, tile, players,
    and state of the game
    @returns whether the given argument is of acceptable player type if not
    indicates the player is invalid
*/
ErrorCode validate_player_type(char* player1, char* player2, Game* game) {
    int validPlayers = 0;
    char validType[3][2] = {"h", "1", "2"};

    // Check for each valid type defined above against both player types given
    for (int typeIndex = 0; typeIndex < 3; typeIndex++) {
        char* currentType = validType[typeIndex];
        PlayerType currentPlayer = typeIndex;

        // Adds number of valid players by one whenever there is a match
        if (strcmp(currentType, player1) == 0) {
            game->player1->type = currentPlayer;
            game->player1->symbol = "*";
            validPlayers++;     
        }
        if (strcmp(currentType, player2) == 0) {
            game->player2->type = currentPlayer;
            game->player2->symbol = "#";
            validPlayers++;
        }
        
    }

    // number of matching valid players is not two then it is invalid
    if (validPlayers != 2) {
        return PLAYER_INVALID;
    }
    
    return NO_ERROR;
}

/*
    Initialises a player object at the start of the game

    @param firstPlayer indicates whether this Player object is the first to 
    be defined
    @return gives a new player object for use in the game
*/
Player* initialise_player(bool firstPlayer) {
    Player* newPlayer = malloc(sizeof(Player));
    newPlayer->firstPlayer = firstPlayer;
    newPlayer->recentMove[0] = INT_MIN;
    newPlayer->recentMove[1] = INT_MIN;
    return newPlayer;
}

/*
    Frees the player object after the end of game or an error is encountered
    during parsing.
    
    @param gameContent object containing game information
*/
void free_players(Game* gameContent) {
    free(gameContent->player1);
    free(gameContent->player2);
}