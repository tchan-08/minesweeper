#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef enum Status {
    WIN,
    LOSE,
    ERROR
} Status;

typedef struct cell {
    bool revealed;
    bool flagged;
    bool isBomb;
    int value;
} cell;

typedef struct game {
    int rows;
    int cols;
    int bombs;
    cell **board;
} game;


cell **createBoard(int rows, int cols) {
    if (rows < 2 || cols < 2) {
        printf("dimensions not big enough");
        return NULL;
    }
    cell **board = malloc(rows * sizeof(cell *));
    for (int i = 0; i < rows; i++) {
        board[i] = calloc(cols, sizeof(cell));
    }
    return board;
}

void fillBoard(cell **board, int rows, int cols, int bombs) {
    int bombsPlaced = 0;
    while (bombsPlaced < bombs) {
        int chosenRow = rand() % rows;
        int chosenColumn = rand() % cols;
        if (!board[chosenRow][chosenColumn].isBomb) {
            board[chosenRow][chosenColumn].isBomb = true;
            bombsPlaced++;
        }
    }
}

void assignCellValues(cell **board, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int count = 0;
            if (!board[i][j].isBomb) {
                //TL: i-1, j-1
                if (i != 0 && j != 0) if (board[i-1][j-1].isBomb) count++; //check top left of cell
                if (i != 0) if (board[i-1][j].isBomb) count++; //check above cell
                if (i != 0 && j != cols-1) if (board[i-1][j+1].isBomb) count++; //check top right of cell
                if (j != cols-1) if (board[i][j+1].isBomb) count++; //check right of cell
                if (i != rows-1 && j != cols-1) if (board[i+1][j+1].isBomb) count++; //check bottom right of cell
                if (i != rows-1) if (board[i+1][j].isBomb) count++; //check below cell
                if (i != rows-1 && j != 0) if (board[i+1][j-1].isBomb) count++; //check bottom left of cell
                if (j != 0) if (board[i][j-1].isBomb) count++;
                board[i][j].value = count;
            }
        } 
    }
}

game *initialiseGame(int rows, int cols, int bombs) {
    game *game = malloc(sizeof(*game));
    game->rows = rows;
    game->cols = cols;
    game->bombs = bombs;
    game->board = createBoard(game->rows, game->cols);
    fillBoard(game->board, game->rows, game->cols, game->bombs);
    assignCellValues(game->board, game->rows, game->cols);
    return game;
}

void printBoard(game *game) {
    int rows = game->rows;
    int cols = game->cols;
    cell **board = game->board;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i][j].revealed) {
                if (board[i][j].isBomb) {
                    printf("[X]");
                } else if (board[i][j].flagged) {
                    printf("[F]");
                } else {
                    if (board[i][j].value < 1) {
                        printf("[ ]");
                    } else {
                        printf("[%d]", board[i][j].value);
                    }
                }
            } else {
                printf("[#]");
            }
        }
        printf("\n");
    }
}

Status revealCell(game *game, int chosenRow, int chosenCol) {
    int rows = game->rows;
    int cols = game->cols;
    cell **board = game->board;
    if ((chosenRow < 0 || chosenRow > rows) || (chosenCol < 0 || chosenCol > cols)) {
        return ERROR;
    }
    if (board[chosenRow][chosenCol].isBomb) {
        return LOSE;
    } else {

    }

}

void revealEmpty(game *game, int row, int col) {
    if (row != 0 && col != 0) { //search TL cell
        if (game->board[row-1][col-1].value != 0) { //reveal boundary cells
            game->board[row-1][col-1].revealed = true;
        } else if (game->board[row-1][col-1].value == 0 && !game->board[row-1][col-1].revealed) { //recurse through 0-cells
            game->board[row-1][col-1].revealed = true;
            revealEmpty(game, row-1, col-1);
        }
    }
    if (row != 0) { //search cell above
        if (game->board[row-1][col].value != 0) { //reveal boundary cells
            game->board[row-1][col].revealed = true;
        } else if (game->board[row-1][col].value == 0 && !game->board[row-1][col].revealed) { //recurse through 0-cells
            game->board[row-1][col].revealed = true;
            revealEmpty(game, row-1, col);
        }
    }
    if (row != 0 && col != game->cols-1) {
        if (game->board[row-1][col+1].value != 0) { //reveal boundary cells
            game->board[row-1][col+1].revealed = true;
        } else if (game->board[row-1][col+1].value == 0 && !game->board[row-1][col+1].revealed) { //recurse through 0-cells
            game->board[row-1][col+1].revealed = true;
            revealEmpty(game, row-1, col+1);
        }
    }
} 

int main() {
    srand(time(NULL));
    game *game = initialiseGame(10, 10, 10);
    printBoard(game);
    return 0;
}