#include "grid.h"

/* 
    Function to draw grid before the next turn by the players.

    @param gameBoard the pointer that reflects the existing placements made 
*/
void grid_display(Board* gameBoard) {
    Player*** grid = gameBoard->grid;
    int width = gameBoard->width;
    int height = gameBoard->height;

    // Iterating each rows and columns to print the player symbol at that place
    for (int rows = 0; rows < height; rows++) {
        for (int cols = 0; cols < width; cols++) {
            Player* currentPosition = grid[rows][cols];
            if (currentPosition != NULL) {
                printf("%s", currentPosition->symbol);
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

/* 
    Reallocates grid from 1x1 to user defined dimension, once arguments are
    validated to be correct.

    @param grid contains the existing placements
    @param height contains information about board height
    @param width contains informaton about board width
*/
void reallocate_grid(Board* grid, int height, int width) {

    // Reallocates the number of rows in the board to new value
    grid->grid = realloc(grid->grid, sizeof(Player**) * height);

    // For each row reallocating the number of columns in that row
    for (int eachRow = 0; eachRow < height; eachRow++) {
        // Weird? Just pleasing valgrind so that reallocation only happens
        // if the memory is allocated in the first place
        if (eachRow < grid->height) {
            grid->grid[eachRow] = 
                    realloc(grid->grid[eachRow], sizeof(Player*) * width);
        } else {
            grid->grid[eachRow] = malloc(sizeof(Player*) * width);
        }
        // Defining each column to be NULL 
        for (int eachCol = 0; eachCol < width; eachCol++) {
            grid->grid[eachRow][eachCol] = NULL;
        }     
    }
    // Changing the dimension information of the board
    grid->width = width;
    grid->height = height;
}

/* 
    Checks whether grid contains only acceptable characters as the saved game
    is read character by character. If valid, the corresponding player pointer
    is set in the row and column of the board. 

    @param row the current row being read in the saved file
    @param col the current column being read in the saved file
    @param value the current value in the saved file
    @param game to modify the grid content
    @returns whether the grid content is valid and contains correct dimension
*/
bool valid_grid_content(int row, int col, char value, Game* game) {
    Board* board = game->gameBoard;
    int maxWidth = board->width;
    int maxHeight = board->height;
   
    Player*** coordinate = board->grid;
    Player* player1 = game->player1;
    Player* player2 = game->player2;
    
    // If the current row or column exceeds the dimension then indicates it as
    // an invalid file
    if (row >= maxHeight || col >= maxWidth) {
        return false;
    }

    // Checks whether value is of acceptable type and assigns the value
    // at the respective position. Returns false if contains any unacceptable
    // characters
    if (value == '#') {
        coordinate[row][col] = player2;
    } else if (value == '*') {
        coordinate[row][col] = player1;
    } else if (value == '.') {
        coordinate[row][col] = NULL;
    } else {
        return false;
    }
    // If here then all invalidities does not apply, hence content is valid
    return true;
}

/* 
    Initialises a grid structure in memory used at the start of every game.
    Initially the grid is defined as 1x1 with its value pointing to NULL.

    @returns a board loaded with default values
*/
Board* initialise_grid(void) {
    Board* newGrid = malloc(sizeof(Board));
    newGrid->width = 1;
    newGrid->height = 1;
    // Initially it is a 1x1 array
    newGrid->grid = malloc(sizeof(Player**) * 1);
    newGrid->grid[0] = malloc(sizeof(Player*) * 1);
    // Defining the only element to be NULL
    newGrid->grid[0][0] = NULL;
    return newGrid;
}

/* 
    To free the memory used by the grid after an error or end of game.
    
    @param grid contains the Board data structure holding information about
    game and dimension.
*/
void free_grid(Board* grid) {
    int rows = grid->height;

    // Free each column
    for (int y = 0; y < rows; y++) {
        free(grid->grid[y]);
    }
    // Free each row and the Board data structure
    free(grid->grid);
    free(grid);
}
