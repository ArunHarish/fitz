#include "token.h"

/* 
    Allocates memory for token input during any parsing for an expected number
    of tokens, with each having a specific character. 

    @param tokenLength the expected number of token for an usage
    @param characterLength the expected length of each tokens
    @return tokens containing allocated character length
*/
char** create_tokens(int tokenLength, int characterLength) {
    char** tokens = malloc(sizeof(char*) * tokenLength);
    for (int tokenIndex = 0; tokenIndex < tokenLength; tokenIndex++) {
        tokens[tokenIndex] = malloc(sizeof(char) * characterLength);
    }
    return tokens;
}

/* 
    Validates whether the tokens are proper for an user input. 

    @param tokens contain the list of allocated tokens using the above
    function
    @param command a list that holds the converted numeric value after
    function execution. 
    @return valid_integer indicates whether the token is valid using TokenCode 
    from the function convert_token_to_int
*/
TokenCode validate_tokens(char** tokens, int* command) {
    const char* save = "save";
    const int firstLength = strlen(tokens[0]);
    const char* temp = strstr(tokens[0], save);
    
    // If the word save is the first word in the input and the content 
    // following it is same as the entire input then it is valid
    if (tokens[0] != NULL && temp != NULL && strcmp(temp, tokens[0]) == 0) {
        // If the second token is not empty that indicates there was more
        // content than required
        if (strlen(tokens[1]) > 0) {
            return INVALID_INPUT;
        }
        // Here all the values of first token after the word save is copied 
        // to the second token as required by the save_game function
        reallocate_token(tokens, 1, strlen(tokens[0]));
        for (int x = 4; x <= firstLength; x++) {
            if (x == firstLength) {
                tokens[1][x - 4] = '\0';
            } else {
                tokens[1][x - 4] = tokens[0][x];
            }
        }
         
        return SAVE_FILE;
    }

    TokenCode validInteger = convert_token_to_int(tokens, command, 3);
    return validInteger;
}

/* 
    Reallocates the token length if the user input exceeds 10 characters

    @param token a list of tokens allocated before
    @param tokeIndex expected number of tokens already defined
    @param tokenSize character length of each tokens
*/
void reallocate_token(char** token, int tokenIndex, int tokenSize) {
    // Adding five more to the tokenSize for assurance
    token[tokenIndex] = 
            realloc(token[tokenIndex], sizeof(char) * (tokenSize + 5));
}

/* 
    Frees the token after end of game due to error or player won. 

    @param tokens contains the list of tokens allocated during parsing
    @param tokenLength indicates total token length
*/
void free_tokens(char** tokens, int tokenLength) {
    for (int tokenIndex = 0; tokenIndex < tokenLength; tokenIndex++) {
        free(tokens[tokenIndex]);
    }
    free(tokens);
}

/* 
    Converts the input into tokens or chunks to check their validity.
    Helper function for tokenise_input function defined below

    @param tokenList token defined using create_tokens
    @param string the content to be parsed and extract tokens from
    @param tokenLength expected number of tokens from the string 
*/
void tokenise_string(char** tokenList, char* string, int tokenLength) {
    const char* delimiter = " ";
    int totalToken = 0;
    char* currentToken;
    
    int possibleTokens = 0; 
    int spaceChecker = 0;
    
    // This is to check the total possible token from the input
    do {
        char current = string[spaceChecker];
        if (current == ' ') {
            possibleTokens++;   
        }
        spaceChecker++;
    } while (string[spaceChecker] != '\0'); 

    // Getting first token match
    currentToken = strtok(string, delimiter); 
    /*  
        Moving to the next token, if the number of matched token is lesser than
        expected number of tokens and possible tokens is lesser than expected 
        tokens. 
        E.g. if "a b c" then possible token is two and expected token
        could be three and never exceeds the possible token.
        This is to avoid trailing, leading and any unnecessary intermittent
        spaces.
    */
    while (currentToken != NULL && totalToken < tokenLength &&
            possibleTokens < tokenLength) {
        int tokenLength = strlen(currentToken);
        // If token exceeds 10 characters, could be a save command with big
        // path, then reallocates the current token length only
        if (tokenLength > DEFAULT_TOKEN_SIZE) {
            reallocate_token(tokenList, totalToken, tokenLength);        
        }
        // Copies the current token into a token array with corresponding index
        strcpy(tokenList[totalToken], currentToken);
        totalToken++;
        currentToken = strtok(NULL, delimiter);
    }  
    // For remaining tokens setting it to be empty
    if (totalToken < tokenLength) {
        for (; totalToken < tokenLength; totalToken++) {
            strcpy(tokenList[totalToken], "");
        }
    }
}

/* 
    Converts user input into string to parse their values or check their 
    validity. 

    @param tokenList to contain the list of tokens to be evaluated
    @param tokenLength expected number of tokens
    @returns whether the conversion into tokens was successful
*/
bool tokenise_input(char** tokenList, int tokenLength) {
    char input[MAX_USER_INP];
    int currentCharacter = '\0';
    int position = 0;

    // Iterating through each character of the user input
    while (position < MAX_USER_INP - 1) {
        currentCharacter = fgetc(stdin);
        // If EOF or Ctrl+D is pressed must return input as invalid 
        if (feof(stdin)) {
            return false;
        // If current character is a line terminator indicates end of input
        } else if (currentCharacter == '\n' || position == MAX_USER_INP - 1) {
            input[position++] = '\0';
            break;
        } else {
            input[position++] = currentCharacter;
        }       
    }
    // Converts input into chunks or tokens
    tokenise_string(tokenList, input, tokenLength);
    return true;
}

/* 
    Converts dimension or coordinate tokens into their numeric values
    Returns TokenCode stating its success.
    
    @param token contains the list of tokens to be converted
    @param array the list of numeric values to be assigned numeric values to
    @param tokenLength the expected number of tokens
    @returns the token error code indicating the validity of conversion
*/
TokenCode convert_token_to_int(char** token, int* array, int tokenLength) {
    for (int tokenIndex = 0; tokenIndex < tokenLength; tokenIndex++) {
        char* currentToken = token[tokenIndex];
        if (strlen(currentToken) == 0) {
            return INVALID_INPUT;
        }
        // Transform it into a valid number comparing it with
        // minimum and maximum possible boundary value of the integer type
        char* nonNumeric;
        long value = strtol(currentToken, &nonNumeric, 10);
        if (value <= INT_MIN || value >= INT_MAX || *nonNumeric) {
            return INVALID_INPUT;
        } else {
            array[tokenIndex] = (int) value;
        }
    }
    return NO_TOKEN_ERROR;
}