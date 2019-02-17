#include "parser.h"
#include "game.h"

/* 
    Function that takes the error code and displays matching error. 
    
    @param exitCode indicates the index and exit code the game must exit with.
*/
void display_errors(ErrorCode exitCode) {
    // If no error nothing to do
    if(exitCode == NO_ERROR) {
        return;
    }
    char errorMessage[8][70] = {
        "Usage: fitz tilefile [p1type p2type [height width | filename]]",
        "Can't access tile file",
        "Invalid tile file contents",
        "Invalid player type",
        "Invalid dimensions",
        "Can't access save file",
        "Invalid save file contents",
        "End of input"
    };
    // Since EOF_DETECTED has a different exit code
    if (exitCode == EOF_DETECTED) {
        fprintf(stderr, "%s\n", errorMessage[7]);
    } else {
        fprintf(stderr, "%s\n", errorMessage[exitCode - 1]);
    }
}

/* 
    Validates whether the header of the loading file is valid and assigns the
    values to the defined Game object.

    @param firstLine the input string read from the saved file
    @param game the object to assign parsed values into
    @returns whether parsing was flawless
*/
bool parse_header(char* firstLine, Game* game) {
    int tokenLength = 4;
    char** tokens = create_tokens(tokenLength, 10);
    Tiles* tile = game->tiles;
    // Convert the input into tokens
    tokenise_string(tokens, firstLine, tokenLength);

    int headerInfo[tokenLength];
    int totalTiles = tile->total;

    // For each token 
    for (int x = 0; x < tokenLength; x++) {
        if (strlen(tokens[x]) == 0) {
            free_tokens(tokens, tokenLength);
            return false;
        }
    }
    // Convert the token to a numeric value to contain information such as
    // who's turn, dimension of the grid, and the current tile index
    convert_token_to_int(tokens, headerInfo, tokenLength);
    
    int tileIndex = headerInfo[0];
    int gameTurn = headerInfo[1];
    int gameHeight = headerInfo[2];
    int gameWidth = headerInfo[3];
    // Conditions for header to be invalid
    if (tileIndex < 0 || tileIndex >= totalTiles || gameTurn < 0
            || gameTurn > 1 || gameHeight > 999 || gameHeight < 1
            || gameWidth > 999 || gameWidth < 1) {
        free_tokens(tokens, tokenLength);
        return false;
    }
    
    tile->current = tileIndex;
    game->turn = gameTurn;
    // Reallocates the grid from 1x1 to gameHeightxgameWidth
    reallocate_grid(game->gameBoard, gameHeight, gameWidth);  
    free_tokens(tokens, tokenLength); 
    return true;
}

/* 
    Takes the game object and converts it into string and saves it to the file
    name described.

    @param game the state which must be saved into a file
    @param token contains information about save path
*/
void save_game(Game* game, char** token) {
    FILE* gameWrite = fopen(token[1], "w");
    
    if (gameWrite == NULL) { 
        fprintf(stderr, "Unable to save game\n");
        return;
    }

    int currentTile = game->tiles->current;
    int currentTurn = game->turn;

    Board* board = game->gameBoard;
    int boardWidth = board->width;
    int boardHeight = board->height;
    // Writing the header information first
    fprintf(gameWrite, "%d %d %d %d\n", currentTile, currentTurn, boardHeight,
            boardWidth);
    fflush(gameWrite);
    // For each row and each column of the grid writing the corresponding
    // content
    for (int rows = 0; rows < boardHeight; rows++) {
        for (int cols = 0; cols < boardWidth; cols++) {
            Player* currentPlace = board->grid[rows][cols];
            // Write the proper string depending on the player type
            if (currentPlace == NULL) {
                fprintf(gameWrite, ".");
            } else {
                fprintf(gameWrite, "%s", currentPlace->symbol);
            }
        }
        // Line termination for each row
        fprintf(gameWrite, "\n");
    }
    fclose(gameWrite);
}

/* 
    Checks whether the game content has the right rows, cols and returns
    corresponding error code.

    @param row the row reached after validating the board from saved game
    @param maxHeight the maximum height of the board
    @returns if the row is not same as maximum height, after exiting the loop, 
    it indicates the file is invalid, or else no error
*/
ErrorCode check_final_dim(int row, int maxHeight) {
    if (row != maxHeight) {
        return FILE_INVALID;
    }
    return NO_ERROR; 
}

/*
    Function to check whether the column is the maximum possible value; not
    short or long.

    @param col the column after each line termination
    @param maxCol the width of the board used as zero indexed for validation
    @returns whether the column before the line termination is valid
*/
bool check_col(int col, int maxCol) {
    if (col <= maxCol - 1) {
        return false;
    }
    return true;
}

/*
    Loads the game from the file given and convert it into game objects

    @param fileName the path of the file name containing game state information
    @param game the object to load the information into
    @returns indicates whether the file is valid, accessible and has correct
    dimension
*/
ErrorCode load_saved_game(char* fileName, Game* game) {
    FILE* savedFile = fopen(fileName, "r");    
    if (savedFile == NULL) {
        return FILE_INACCESS;
    }
    // creating pointers to board width and height to assign their values
    Board* grid = game->gameBoard;
    int* maxWidth = &(grid->width);
    int* maxHeight = &(grid->height);
    // Setting current character to be an invalid value initially
    int currentCharacter = '\0';
    bool isFirstLine = true;
    char* firstLine = malloc(sizeof(char) * 5);
    int row = 0;
    int col = 0;
    // If the current character is end of file terminate
    while ((currentCharacter = fgetc(savedFile)) != EOF) {
        if (currentCharacter == '\n') {
            if (isFirstLine) {
                // Setting last character of first line to be null terminated
                firstLine[col] = '\0';
                if (!parse_header(firstLine, game)) {
                    free(firstLine);
                    return FILE_INVALID;
                }
                row = 0;
                isFirstLine = false;
            } else if (!check_col(col, *maxWidth)) {
                return FILE_INVALID;
            } else {
                row++;
            }
            col = 0;
            continue;
        }
        // If first line then reallocate first line to be always have
        // space enough for give more characters for assurance
        if (isFirstLine) {
            firstLine = realloc(firstLine, sizeof(char) * (col + 5));
            firstLine[col] = currentCharacter;
        } else if (!valid_grid_content(row, col, currentCharacter, game)) { 
            free(firstLine);
            return FILE_INVALID;
        }
        col++;
    }
    free(firstLine); 
    fclose(savedFile);
    ErrorCode checkDimension = check_final_dim(row, *maxHeight);
    return checkDimension;
}

/*
    Validates whether the given rotation is valid and one of 0, 90, 180 and 270

    @param command contains the list of user input parsed: row, col and
    rotation
    @returns indicates whether the given rotation matches the valid rotation
*/
bool correct_rotation(int* command) {
    int rotation[POSSIBLE_ROTATION] = {0, 90, 180, 270};
    for (int rotationIndex = 0; rotationIndex < POSSIBLE_ROTATION; 
            rotationIndex++) {
        if (rotation[rotationIndex] == command[2]) {
            return true;
        }
    }
    return false;
}

/*
    Gets the input from the user, parses it and validates them, and if all good
    it then creates the move 

    @param game the game object in which the move is made on to
    @param currentPlayer the player who makes the move to change the board
    @param command the list in which the value must be assigned to
    @returns whether the player has made a valid move 
*/
bool human_prompt(Game* game, Player* currentPlayer, int* command) {
    Tiles* currentTile = game->tiles;
    Board* grid = game->gameBoard;
    int tokenLength = 3;
    while (true) {
        char** tokens = create_tokens(tokenLength, 10);
        // Print stuff
        fprintf(stdout, "Player %s] ", currentPlayer->symbol);
        fflush(stdout);
        // Get token
        if (!tokenise_input(tokens, tokenLength)) {
            free_tokens(tokens, tokenLength);
            return false;
        }
        // Validate token: if save valid then calls the function to save
        // else validates whether the given rotation input is valid and 
        // performs validation of the given move.
        TokenCode errorCode = validate_tokens(tokens, command);
        if (errorCode == SAVE_FILE) {
            save_game(game, tokens);
        } else if (errorCode == NO_TOKEN_ERROR && correct_rotation(command)
                && valid_move(command, currentPlayer, grid, currentTile)) {
            place_move(command, currentPlayer, game, currentTile);
            change_turn(game);
            change_tiles(game);
            free_tokens(tokens, tokenLength);
            break;
        }
        free_tokens(tokens, tokenLength);
    }
    return true;
}

/*
    To display the move made by the algorithm player.

    @param command the move made by the algorithm along with rotation selection
    @param currentPlayer the automatic player to get their symbol for display
*/
void algorithm_output(int* command, Player* currentPlayer) {
    int row = command[0];
    int col = command[1];
    int rotation = command[2];
    printf("Player %s => %d %d rotated %d\n", currentPlayer->symbol, row, col, 
            rotation);
}

/*
    Validates the dimension parsed when reading the saved file.

    @param height the token that contains user input about the height of the 
    board
    @param width the token that contains user input about the width of the 
    board
    @param game to change the board size once the dimension is validated
    @returns whether the dimension is valid after parsing and indicates
    corresponding error
*/
ErrorCode validate_dimension(char* height, char* width, Game* game) {
    // Using tokens
    char* tokens[2] = {
        height, 
        width    
    }; 
    int dimension[2];
    // Converts the token into numeric value
    TokenCode validDimension = convert_token_to_int(tokens, dimension, 2);
    // If the tokens are not convertible into a numeric value
    if (validDimension == INVALID_INPUT) {
        return DIM_INVALID;
    }

    // The first index is height or row and the second is width or column
    int actualWidth = dimension[1];
    int actualHeight = dimension[0];

    // Checking whether they are within bounds
    if (actualHeight < 1 || actualHeight > 999 || actualWidth < 1 || 
            actualWidth > 999) {
        return DIM_INVALID;
    }
    // Here once validations are good, and reallocates the board
    reallocate_grid(game->gameBoard, actualWidth, actualHeight);
    return NO_ERROR;
}

/*
    Parses the user given argument and validates them and takes decision
    based on the number of arguments given as specified in the display_error

    @param argn the number of arguments provided by the user
    @param input the actual parsed arguments provided by the program
    @returns the ErrorCode that contains the value corresponding to their 
    exit codes
*/
ErrorCode parse_arguments(int argn, char** input) {    
    Game* game = initialise_game();
    Tiles* tiles = game->tiles;

    if (argn == 2) {
        // Show only tiles and their rotated versions
        return show_only_tiles(input[1], tiles);
    } else if (argn == 5 || argn == 6) {
        // Set the player in the game object
        ErrorCode validPlayer = validate_player_type(input[2], input[3], game);
        ErrorCode validTile = validate_tile_file(input[1], tiles);
        if (validTile != NO_ERROR) {
            free_game(game);
            return validTile;
        }
        if (validPlayer == PLAYER_INVALID) {
            free_game(game);
            return PLAYER_INVALID;
        }
    } else {
        return WRONG_PARAM;
    }

    if (argn == 5) {
        // Read saved file
        ErrorCode validFile = load_saved_game(input[4], game);
        if (validFile != NO_ERROR) {
            free_game(game);
            return validFile;
        }
    } else if(argn == 6) {
        // Validate dimension
        ErrorCode validDimension = 
                validate_dimension(input[5], input[4], game);
        if (validDimension != NO_ERROR) {
            free_game(game);
            return validDimension;
        }
        
    }
    // If all clear we start the game
    return game_loop(game); 
}
