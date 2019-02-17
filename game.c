#include "game.h"

/*
    Initiates the game by parsing the arguments given by the user

    @param argc total number of arguments given
    @param argv the actual list of arguments
    @returns gives the exit code of the game
*/
int start_game(int argc, char** argv) {
    // Gets the parsing code from parsing the arguments
    ErrorCode exitCode = parse_arguments(argc, argv);
    // Displays any necessary messages
    display_errors(exitCode);
    return exitCode; 
}

/*
    Initialises the game with default values for players, game grid,
    tiles and sets the recent move to be invalid value that is not possible
    by the dimension constraints i.e. -999 till 999

    @returns the game state to be manipulated by game functions
*/
Game* initialise_game(void) {
    Game* newGame = malloc(sizeof(Game));
    // Initialises players
    newGame->player1 = initialise_player(true);
    newGame->player2 = initialise_player(false);
    // Initialises game grid and tiles
    newGame->gameBoard = initialise_grid();
    newGame->tiles = initialise_tile_list();
    // Assigns value that is not possible by dimension constraints
    newGame->recentMove[0] = INT_MIN;
    newGame->recentMove[1] = INT_MIN;
    // Indicates first player
    newGame->turn = 0;
    return newGame;
}

/*
    Frees the memory allocated for the entire game including tiles, grids,
    players and the actual game

    @param gameContent contains the state of the game and its dependencies
*/
void free_game(Game* gameContent) {
    free_tiles(gameContent->tiles);
    free_grid(gameContent->gameBoard);
    free_players(gameContent);
    free(gameContent);
}

/*
    Changes the turn of the player within the game

    @param game contains the state about which player is currently playing
*/
void change_turn(Game* game) {
    // Changes the turn of the player by incrementing turn and 
    // using modulus of two, zero indicates first player and 
    // one indicates the second
    int turn = game->turn;
    game->turn = (++turn) % 2;
}

/*
    Places the move in the given coordinate by reading the content of tiles,
    and placing the content based on who is playing. It assumes that the given
    coordinate is valid and places values, a helper function for valid_move
    function.

    @param command contains the coordinate and rotation input from the user
    @param playing the player who is currently in their turn
    @param game contains the game state
    @param tiles used to get the tile layout for placement
*/
void place_move(int* command, Player* playing, Game* game, Tiles* tiles) { 
    int centerY = command[0];
    int centerX = command[1];

    int offset = TILE_GRID_SQR_DIM / 2;
    int currentIndex = tiles->current;
    
    // Each rotated tile is stored in the memory
    int rotationIndex = command[2] / 90;
    int** currentTile = tiles->grid[rotationIndex][currentIndex];
    
    Board* grid = game->gameBoard;
    Player*** board = grid->grid;

    // Iterates from the top of the tile in the perspective of the
    // board, the centerY - offset to the bottom of the tile in the
    // perspective of the board.
    for (int y = centerY - offset; y <= centerY + offset; y++) {
        // Getting the vertical index from the tiles perspective
        int tileY = y - centerY + offset;
        // Iterates from the left most corner to the other end
        for (int x = centerX - offset; x <= centerX + offset; x++) {
            // Getting the horizontal index from the tiles perspective
            int tileX = x - centerX + offset;
            // If the tile at that coordinate exist then place the player
            if (currentTile[tileY][tileX]) {
                board[y][x] = playing;
            }
        } 
    }
    
    // Update to the recent move of the game and the player for its use in
    // algorithms
    game->recentMove[0] = centerY; 
    playing->recentMove[0] = centerY;
    game->recentMove[1] = centerX;
    playing->recentMove[1] = centerX;
}

/*
    Validates whether the given coordinate and current tile can be placed on 
    the board.

    @param command contains the coordinate and rotation input from the player
    @param playing who is currently playing the game
    @param grid the board that contains the placements
    @param tiles containing tiles and the current tile
    @returns whether the move was valid by the player
*/
bool valid_move(int* command, Player* playing, Board* grid, Tiles* tiles) {
    int centerY = command[0];
    int centerX = command[1];
    // Getting the mid-point of the tile grid since it is always a square
    int offset = TILE_GRID_SQR_DIM / 2;
    // Getting the rotation index
    int rotationIndex = command[2] / 90;
    int currentIndex = tiles->current;
    int** currentTile = tiles->grid[rotationIndex][currentIndex];
    
    int maxWidth = grid->width;
    int maxHeight = grid->height;
    
    Player*** board = grid->grid;
     
    for (int y = centerY - offset; y <= centerY + offset; y++) {
        int tileY = y - centerY + offset;
        for (int x = centerX - offset; x <= centerX + offset; x++) {
            int tileX = x - centerX + offset;
            // If any placeable points in the tile is outside of the board 
            // bounds or, the board already contains the player then the move 
            // is invalid
            if (currentTile[tileY][tileX] && (y < 0 || y >= maxHeight ||
                    x < 0 || x >= maxWidth || board[y][x] != NULL)) {
                return false;
            }

        }
    }
    return true;
}

/*
    Updates the row and column of the current position as required by
    Type 1 algorithm

    @param *row the value of the row to be changed for algorithm one
    @param boardHeight the maximum row + 1 of the board
    @param *col the value of the column to be changed for algorithm one
    @param boardWidth the maximum column + 1 of the board
*/
void update_position_algorithm_one(int* row, int boardHeight, int* col,
        int boardWidth) {
    
    // Stores the current row and column of the iterator
    int currentRow = *(row);
    int currentCol = *(col);

    currentCol++;
    // If the current column goes over the possible column + 1
    // Then reset column to -2
    if (currentCol > (boardWidth - 1) + 2) {
        currentCol = -2;
        currentRow++;
    }
    // If the current row exceeds the possible row + 1
    // Then reset row to -2
    if (currentRow > (boardHeight - 1) + 2) {
        currentRow = -2;
    }
    // Assigns the new position value to row and column
    *(row) = currentRow;
    *(col) = currentCol;
}

/*
    Type 1 algorithm for automatic player that checks every possible
    combination to find a valid one.
    The algorithm firstly iterates through each possible rotation and for
    each rotation checks all possible moves resetting as defined in above
    function until reaching the initial move.

    @param game contains the state of the game to get information about turn
    and board, to change 
    @param totalPlayers contains both the player objects
*/
void algorithm_one(Game* game, Player** totalPlayers) {
    int turn = game->turn;
    int recentMove[2] = {
        game->recentMove[0],
        game->recentMove[1] 
    };

    Board* board = game->gameBoard;
    Tiles* tiles = game->tiles;
    Player* currentPlayer = totalPlayers[turn];
    
    int boardWidth = board->width;
    int boardHeight = board->height;

    // If no move has been made yet, indicated by the minimum of the integer
    // type
    if (recentMove[0] == INT_MIN && recentMove[1] == INT_MIN) {
        recentMove[0] = -2;
        recentMove[1] = -2;
    }

    int row = recentMove[0];
    int col = recentMove[1];
    int theta = 0;

    //Change theta by 90 until the new theta is same as the current theta
    do {
        // Change position until the new move is same as the current move
        do {
            int command[3] = {
                row,
                col,
                theta
            };
            if (valid_move(command, currentPlayer, board, tiles)) {
                place_move(command, currentPlayer, game, tiles);
                algorithm_output(command, currentPlayer);
                change_turn(game);
                change_tiles(game);
                return;
            }
            
            update_position_algorithm_one(&row, boardHeight, &col, boardWidth);
    
        } while (row != recentMove[0] || col != recentMove[1]);
        // Change theta by 90
        theta += 90;
    } while (theta <= 270);

}

/*
    Updates the row and column as required by Type 2 algorithm
    
    @param *row the value of the row to be changed for algorithm two
    @param boardHeight the maximum row + 1 of the board
    @param *col the value of the column to be changed for algorithm tow
    @param boardWidth the maximum column + 1 of the board
    @param isFirstPlayer indicates whether the player is teh first player or
    otherwise for algorithm implementation
*/
void update_position_algorithm_two(int* row, int boardHeight, int* col, 
        int boardWidth, bool isFirstPlayer) {
    int currentRow = *(row);
    int currentCol = *(col);

    // If first player then search from left to right
    // and top-to-bottom and alternative for second player
    if (isFirstPlayer) {
        currentCol++;
        // If column exceeds max col + 1
        if (currentCol > (boardWidth - 1) + 2) { 
            currentCol = -2;
            currentRow++;
        }
        // If row exceeds max row + 1
        if (currentRow > (boardHeight - 1) + 2) {
            currentRow = -2;
            currentCol = -2;
        }
    } else {
        currentCol--;
        // If column is smaller than -2
        if (currentCol < -2) {
            currentCol = (boardWidth - 1) + 2;
            currentRow--;
        }
        // If row is smaller than -2
        if (currentRow < -2) {
            currentRow = (boardHeight - 1) + 2;
            currentCol = (boardWidth - 1) + 2;
        }
    }
    // Assigning the new value to the row and column in the algorithm type 2
    *(row) = currentRow;
    *(col) = currentCol;
}

/* 
    Implements the Type 2 algorithm that takes the recent move by the player
    and wraps around based on the player type: if the player is the first
    player it wraps from left to right and top to bottom and wraps from right
    to left and bottom to top, if otherwise.

    @param game contains the state of the game to manipulate tiles, and grids
    @param currentPlayer the automatic player Type 2
*/
void algorithm_two(Game* game, Player* currentPlayer) {
    int recentMove[2] = {
        (currentPlayer->recentMove[0]),
        (currentPlayer->recentMove[1])
    };

    bool firstPlayer = currentPlayer->firstPlayer;
    Board* board = game->gameBoard;
    Tiles* tiles = game->tiles;    

    int boardWidth = board->width;
    int boardHeight = board->height;
    
    // Implement second player only
    if (recentMove[0] == INT_MIN && recentMove[1] == INT_MIN) {
        if (firstPlayer) {
            recentMove[0] = -2;
            recentMove[1] = -2;
        } else {
            recentMove[0] = boardWidth + 2;
            recentMove[1] = boardHeight + 2; 
        }
    }
    
    int row = recentMove[0];
    int col = recentMove[1];
    
    do {
        int theta = 0;
        do {
            int command[3] = {
                row,
                col, 
                theta
            };
            if (valid_move(command, currentPlayer, board, tiles)) {
                place_move(command, currentPlayer, game, tiles);
                algorithm_output(command, currentPlayer);
                change_turn(game);
                change_tiles(game);
                return;
            }
            theta += 90;
        } while (theta <= 270);

        update_position_algorithm_two(&row, boardHeight, &col, boardWidth, 
                firstPlayer);
    } while (row != recentMove[0] || col != recentMove[1]);
}

/*
    Checks whether there exist any possible move by the current player
    This player can either be automated or human.

    @param game contains the board, and tiles
    @param currentPlayer the player to validate placements in the board
    @returns whether there are any possible moves that can be made
*/
bool check_possible_move(Game* game, Player* currentPlayer) {
    int recentMove[2] = {
        currentPlayer->recentMove[0],
        currentPlayer->recentMove[1]
    };

    Board* board = game->gameBoard;
    Tiles* tiles = game->tiles;

    int boardWidth = board->width;
    int boardHeight = board->height;

    // Assuming the current player is first player to search comprehensively
    if (recentMove[0] == INT_MIN && recentMove[1] == INT_MIN) {
        recentMove[0] = -2;
        recentMove[1] = -2;
    }

    int row = recentMove[0];
    int col = recentMove[1];
    
    // Same as algorithm type 2 implementation
    do {
        int theta = 0;
        do {
            int command[3] = {
                row, 
                col, 
                theta
            };
            
            if (valid_move(command, currentPlayer, board, tiles)) {
                return true;
            }

            theta += 90;
        } while (theta <= 270);
        
        update_position_algorithm_two(&row, boardHeight, &col, boardWidth, 
                true);
  
    } while (row != recentMove[0] || col != recentMove[1]);
    
    return false;
}

/*
    A main loop that continues to iterate unless input ended voluntarily or
    the game has ended. It takes human input, calls the automated algorithm,
    and takes decision on action based on the player type. E.g. displaying
    tiles if the player is a human.

    @param game contains the player, tile and board object enabling 
    @returns whether the game has been ended or stopped voluntarily by the user
*/
ErrorCode game_loop(Game* game) {
    Player* players[2] = {
        game->player1,
        game->player2
    };
    // Game loop
    while (true) {
        int command[3];
        int turn = game->turn; 
        Player* currentPlayer = players[turn];
        // To get previous player implementing change_turn function
        Player* prevPlayer = players[(turn + 1) % 2];
        Board* gameBoard = game->gameBoard;     
        // Displays the grid regardless of the type defined   
        grid_display(gameBoard);
        // check whether there are any valid move
        if (!check_possible_move(game, currentPlayer)) {
            printf("Player %s wins\n", prevPlayer->symbol);
            break;
        }
        // Prompt user if current player is human nd displays the tile
        if (currentPlayer->type == HUMAN) {
            display_tiles(game->tiles, true);
            if (!human_prompt(game, currentPlayer, command)) {
                free_game(game);
                return EOF_DETECTED;
            }
        } else if (currentPlayer->type == P1_AUTO) {
            algorithm_one(game, players);
        } else if (currentPlayer->type == P2_AUTO) {
            algorithm_two(game, currentPlayer);
        }

    }
    // Cannot reach here unless game loop terminated because some player won 
    free_game(game);
    return NO_ERROR;
}
