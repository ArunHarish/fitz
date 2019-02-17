#include "tiles.h"
#include "parser.h"

/*
    Initialises the tile list that loads all the tiles with their respective
    rotations. By default it contains four rows corresponding to possible
    rotations: 0, 90, 180, 270
    
    @returns a tile list that holds a tile and its corresponding rotation
    tiles
*/
Tiles* initialise_tile_list(void) {
    // initialise default values of the tile structure
    Tiles* newTile = malloc(sizeof(Tiles));
    newTile->total = 0;
    newTile->current = 0;
    newTile->rotationIndex = 0;
    newTile->grid = malloc(sizeof(int***) * TOTAL_ROTATION);
    return newTile;
}

/*
    Initialises a tile along with its rotation version containing an empty
    5x5 grid by default

    @param tileList the tile on which the new tiles must be placed
*/
void initialise_new_tile(Tiles* tileList) {
    // Increasing total tile length by 1
    int tileIndex = tileList->total++;
    for(int rotationIndex = 0; rotationIndex < TOTAL_ROTATION; 
            rotationIndex++) {
        // Allocating memory for the tile
        // Can use realloc only but that seems to bring few errors
        // Just pleasing valgrind
        if(tileIndex == 0) {
            tileList->grid[rotationIndex] = malloc(sizeof(int**));
        } else {
            tileList->grid[rotationIndex] = realloc(
                    tileList->grid[rotationIndex], 
                    sizeof(int**) * tileList->total
                    );
        }
        int*** rotations = tileList->grid[rotationIndex];
        // Selecting the tile index and allocating memory for rows
        rotations[tileIndex] = malloc(sizeof(int*) * TILE_GRID_SQR_DIM);
        int** currentTile = rotations[tileIndex];
        // Allocating memory for each cols
        for(int rows = 0; rows < TILE_GRID_SQR_DIM; rows++) {
            currentTile[rows] = malloc(sizeof(int) * TILE_GRID_SQR_DIM);
            int* currentRow = currentTile[rows];
            for(int cols = 0; cols < TILE_GRID_SQR_DIM; cols++) {
                currentRow[cols] = 0;
            }
        }
    }
}

/*
    Frees the tile list and all the rotated version of the tiles going 
    by rotation, by tile and then by each rows.

    @param tileList tileList the tile on which the tiles must be freed
*/
void free_tiles(Tiles* tileList) {
    int**** rotateList = tileList->grid;
    for(int eachRotate = 0; eachRotate < TOTAL_ROTATION; eachRotate++) {
        int*** currentRotation = rotateList[eachRotate];
        for(int eachTile = 0; eachTile < tileList->total; eachTile++) {
            int** tileGrid = currentRotation[eachTile];
            for(int eachRow = 0; eachRow < TILE_GRID_SQR_DIM; eachRow++) {
                int* eachCol = tileGrid[eachRow];
                // Free each col
                free(eachCol);
            }
            // Freeing each tile grid
            free(tileGrid);
        }
        // Freeing each 
        free(currentRotation);
    }
    // Freeing all the rotation of a tile
    free(rotateList);
    // Finally freeing the tile list
    free(tileList);
}

/*
    Takes the zero rotated tile and applies the algorithm to copy it into
    all the possible rotations
    
    @param tiles the tile list that got initialised before assigning the 
    zero degree tile
*/
void rotate_tile(Tiles* tiles) {
    // Needs to make a rotated copy of all the tiles using the zero degree
    // tile
    for(int rotateIndex = 1; rotateIndex < TOTAL_ROTATION; rotateIndex++) {
        int*** lastDegree = tiles->grid[rotateIndex - 1];
        int*** otherTiles = tiles->grid[rotateIndex];
        // For each tiles in the rotation index
        for(int tileIndex = 0; tileIndex < tiles->total; tileIndex++) {
            int** otherTilesIndex = otherTiles[tileIndex];
            int** lastDegreeTilesIndex = lastDegree[tileIndex];
            // Since it is always going to be a square grid 
            for(int rows = 0; rows < TILE_GRID_SQR_DIM; rows++) {
                for(int cols = 0; cols < TILE_GRID_SQR_DIM; cols++) {
                    // The rows become the column but on the opposite direction
                    // i.e. Top to bottom of the previous degree tile becomes
                    // Right to left for the current degree
                    otherTilesIndex[cols][TILE_GRID_SQR_DIM - 1 - rows] =
                            lastDegreeTilesIndex[rows][cols]; 
                }
            }
        }
    }
}

/*
    Draws the tile in use for displaying only the tiles or to display the 
    current tile before every human move; the function can be used for both
    purposes

    @param tile contains all the tile layout to be printed 
    @param onlyCurrent flag indicating whether only the current tile must be
    printed
*/
void display_tiles(Tiles* tile, bool onlyCurrent) {
    int totalTiles = tile->total;
    int**** tileList = tile->grid;
    int currentIndex = tile->current;
    // For each tile in the array
    for (int tileIndex = 0; tileIndex < totalTiles; tileIndex++) {
        // To move the iteration to the current index if only current flag
        // is true
        if (onlyCurrent) {
            tileIndex = currentIndex;
        }
        // For each row
        for (int rows = 0; rows < TILE_GRID_SQR_DIM; rows++) {
            // For each rotation type
            for (int index = 0; index < TOTAL_ROTATION; index++) {
                // For each column
                for (int cols = 0; cols < TILE_GRID_SQR_DIM; cols++) {
                    // If the position is placeable
                    if (tileList[index][tileIndex][rows][cols]) {
                        printf("!");
                    } else {
                        printf(",");
                    }
                }
                // If only the current tile must be printed then only the zero
                // degree must be printed
                if (onlyCurrent) {
                    break;
                }
                // Check whether it is the last rotation index, since they
                // don't require a space if they are the last rotation
                if (index < TOTAL_ROTATION - 1) {
                    printf(" ");
                }
            }
            printf("\n");
        }
        // If onlyCurrent index must be printed then break from further 
        // rotations 
        if (onlyCurrent) {
            break;
        }
        // If it is the last rotation then we need to have a new line after
        // drawing all the rotation and tiles
        if (tileIndex < totalTiles - 1) {
            printf("\n");
        }
    }
}

/*
    Validates the tile file and only then would display the tiles, used for
    showing tile only functionality. 

    @param fileName the path of the tile file to be printed
    @param tiles contains the tile list defaulting to a single empty 
    tile initially
    @return the error code given by validate_tile_file function
*/
ErrorCode show_only_tiles(char* fileName, Tiles* tiles) {
    ErrorCode validTile = validate_tile_file(fileName, tiles);
    if (validTile == NO_ERROR) {
        display_tiles(tiles, false);
    }
    // If showing only tiles then game does not proceed any further hence free
    // the memory used by the tiles
    free_tiles(tiles);
    return validTile;
}

/*
    Validates whether the current position exceeds the boundary size and checks
    whether the current character is an acceptable type

    @param row the row of the current position being parsed and validated to be
    within bounds
    @param col the column of the current position being parsed and validated to
    be within bounds
    @param character the current value being read in the iteration
    @param tile contains the content on which the proper characters are 
    translated as placeable position or not.
    @returns boolean whether the validation was successful
*/
bool validate_current_position(int row, int col, int character, 
        int** tile) {
    // If the row and column exceeds the dimension of the tile grid
    if (row >= TILE_GRID_SQR_DIM || col >= TILE_GRID_SQR_DIM) {
        return false;
    } 
    // If the character is not acceptable     
    if (character == ',') {
        tile[row][col] = 0;
    } else if (character == '!') {
        tile[row][col] = 1;
    } else {
        return false;
    } 
    // Here then it is valid
    return true;
}

/*
    Reads the given file and validates the dimension, and the content while
    converting the content into tile object and rotates all the parsed tiles.

    @param fileName the path or file to access the tiles
    @param tiles object on which the parsed content is reflected
    @returns the error code indicating whether the file is invalid, not found
    or parsed correctly
*/
ErrorCode validate_tile_file(char* fileName, Tiles* tiles) {
    FILE* reader = fopen(fileName, "r");
    if (reader == NULL) {
        return TILE_NOT_FOUND;
    }
    // Defining 
    int character = '\0';
    bool lineTerminated = false;
    initialise_new_tile(tiles);    
    int row = 0;
    int col = 0;
    int** tile = tiles->grid[0][tiles->total - 1];
    while (true) {
        character = fgetc(reader);
        if (feof(reader)) {
            // If there is a short row then it is invalid
            if (row < TILE_GRID_SQR_DIM) {
                return TILE_INVALID;
            }
            break;
        } else if (character == '\n') {
            if (lineTerminated) {
                // If there is any subsequent line terminations
                // New Tile is created if valid row is detected
                if (row < TILE_GRID_SQR_DIM) {
                    return TILE_INVALID;
                } 
                initialise_new_tile(tiles);
                row = 0;
                col = 0;
                //Changing the current tile 
                tile = tiles->grid[0][tiles->total - 1];            
            } else {
                if(col < TILE_GRID_SQR_DIM) {
                    return TILE_INVALID; // If the col is lesser than necessary
                }
                col = 0;
                row++;
            }  
            lineTerminated = true;
            continue;
        } else if (!validate_current_position(row, col, character, tile)) {
            return TILE_INVALID;
        }
        col++;
        lineTerminated = false;   
    }
    rotate_tile(tiles); // Calls the rotate function to create rotate copies
    fclose(reader);
    return NO_ERROR;
}

/*
    Changes the tiles in the game, once reaches the end resets back to the 
    beginning

    @param game object contains the tiles, total number of tiles and 
    the current tile.
*/
void change_tiles(Game* game) {
    Tiles* tilesObject = game->tiles;
    int totalTiles = tilesObject->total;
    int currentIndex = tilesObject->current;
    // Simple but efficient way to move around the current tile pointer 
    tilesObject->current = (++currentIndex) % totalTiles;
}