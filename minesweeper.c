#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef enum Status {
    LOSE,
    OK,
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

bool validateLeGame(int rows, int cols, int bombs) {
    if (rows < 2 || cols < 2) {
        printf("Invalid dimensions, rows and columns must be greater than 2.\n");
        return false;
    }
    if (bombs <= 0 || bombs > rows * cols) {
        printf("Invalid bomb count.\n");
        return false;
    } 
    return true;
}

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
        printf("%2d ", i+1);
        for (int j = 0; j < cols; j++) {
            if (board[i][j].flagged) {
                printf("[F] ");
            }
            else if (board[i][j].revealed) {
                if (board[i][j].isBomb) {
                    printf("[X] ");
                } else if (board[i][j].value < 1) {
                    printf("[ ] ");
                } else {
                    printf("[%d] ", board[i][j].value);
                }
            } else {
                printf("[#] ");
            }
        }
        printf("\n");
    }
    printf("  ");
    for (int i = 0; i < cols; i++) {
        printf("%3d ", i+1);
    }
    printf("\n");
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
    if (row != 0 && col != game->cols-1) { //search TR cell
        if (game->board[row-1][col+1].value != 0) { //reveal boundary cells
            game->board[row-1][col+1].revealed = true;
        } else if (game->board[row-1][col+1].value == 0 && !game->board[row-1][col+1].revealed) { //recurse through 0-cells
            game->board[row-1][col+1].revealed = true;
            revealEmpty(game, row-1, col+1);
        }
    }
    if (col != game->cols-1) { //search right cell
        if (game->board[row][col+1].value != 0) { //reveal boundary cells
            game->board[row][col+1].revealed = true;
        } else if (game->board[row][col+1].value == 0 && !game->board[row][col+1].revealed) { //recurse through 0-cells
            game->board[row][col+1].revealed = true;
            revealEmpty(game, row, col+1);
        }
    }
    if (row != game->rows-1 && col != game->cols-1) { //search BR cell
        if (game->board[row+1][col+1].value != 0) { //reveal boundary cells
            game->board[row+1][col+1].revealed = true;
        } else if (game->board[row+1][col+1].value == 0 && !game->board[row+1][col+1].revealed) { //recurse through 0-cells
            game->board[row+1][col+1].revealed = true;
            revealEmpty(game, row+1, col+1);
        }
    }
    if (row != game->rows-1) { //search bottom cell
        if (game->board[row+1][col].value != 0) { //reveal boundary cells
            game->board[row+1][col].revealed = true;
        } else if (game->board[row+1][col].value == 0 && !game->board[row+1][col].revealed) { //recurse through 0-cells
            game->board[row+1][col].revealed = true;
            revealEmpty(game, row+1, col);
        }
    }
    if (row != game->rows-1 && col != 0) { //search BL cell
        if (game->board[row+1][col-1].value != 0) { //reveal boundary cells
            game->board[row+1][col-1].revealed = true;
        } else if (game->board[row+1][col-1].value == 0 && !game->board[row+1][col-1].revealed) { //recurse through 0-cells
            game->board[row+1][col-1].revealed = true;
            revealEmpty(game, row+1, col-1);
        }
    }
    if (col != 0) { //search left cell
        if (game->board[row][col-1].value != 0) { //reveal boundary cells
            game->board[row][col-1].revealed = true;
        } else if (game->board[row][col-1].value == 0 && !game->board[row][col-1].revealed) { //recurse through 0-cells
            game->board[row][col-1].revealed = true;
            revealEmpty(game, row, col-1);
        }
    }
} 

Status revealCell(game *game, int chosenRow, int chosenCol) {
    int rows = game->rows;
    int cols = game->cols;
    cell **board = game->board;
    if ((chosenRow < 0 || chosenRow >= rows) || (chosenCol < 0 || chosenCol >= cols)) {
        return ERROR;
    }
    if (board[chosenRow][chosenCol].isBomb) {
        return LOSE;
    } else {
        if (board[chosenRow][chosenCol].value == 0) {
            board[chosenRow][chosenCol].revealed = true;
            revealEmpty(game, chosenRow, chosenCol);
        } else {
            board[chosenRow][chosenCol].revealed = true;
        }
        return OK;
    }
}

bool checkFlagged(game *game) {
    int bombCount = 0;
    int flagCount = 0;
    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->cols; j++) {
            if (game->board[i][j].flagged) {
                flagCount++;
                if (game->board[i][j].isBomb) {
                    bombCount++;
                }
            }
        }
    }
    return (bombCount == game->bombs && flagCount == game->bombs);
}

void freeGame(game *game) {
    for (int i = 0; i < game->rows; i++) {
        free(game->board[i]);
    }
    free(game->board);
    free(game);
    printf("Game freed\n");
}

void gameLoop(game *game) {
    int flaggedCells = 0;
    while (true) {
        printBoard(game);
        for (int i = 0; i < game->rows; i++) printf("-");
        printf("\n");
        printf("<Flagged cells: %d | Bomb count: %d>\n", flaggedCells, game->bombs);
        bool valid = false;
        bool flagMode = false;
        int choice = 0;
        while (!valid) {
            printf("Options:\n");
            printf("1. Reveal Cell\n");
            printf("2. Flag Cell\n");
            scanf("%d", &choice);
            if (choice == 1 || choice == 2) {
                valid = true;
                if (choice == 2) {
                    printf("Flagging mode\n");
                    flagMode = true; 
                }
            }
        }
        valid = false;
        int row, col;
        while (!valid) {
            printf("Row?\n");
            row = 0;
            scanf("%d", &row);
            row -= 1;
            printf("Column?\n");
            col = 0;
            scanf("%d", &col);
            col -=1;
            if (row < 0 || row >= game->rows || col < 0 || col >= game->cols || game->board[row][col].revealed) {
                printf("Invalid cell\n");
            } else {
                valid = true;
            }
        }
        if (flagMode) {
            if (game->board[row][col].flagged){
                game->board[row][col].flagged = false;
                flaggedCells -= 1;
            } else {
                game->board[row][col].flagged = true;
                flaggedCells += 1;
            }
            if (checkFlagged(game)) {
                printBoard(game);
                printf("You win!\n");
                return;
            }
        } else {
            Status status = revealCell(game, row, col);
            if (status == LOSE) {
                printBoard(game);
                printf("You lose!\n");
                return;
            }
        }
    }
}

void userMenu() {
    printf("MINESWEEPER\n");
    printf("By Epic Tinron Chan...\n");
    printf("----------------------\n");
    bool valid = false;
    while (true) {
        valid = false;
        int rows, cols, bombs;
        while (!valid) {
            printf("How many rows?\n");
            rows = 0;
            scanf("%d", &rows);
            printf("How many columns?\n");
            cols = 0;
            scanf("%d", &cols);
            printf("How many bombs?\n");
            bombs = 0;
            scanf("%d", &bombs);
            if (validateLeGame(rows, cols, bombs)) {
                valid = true;
            } else {
                printf("Error: \n");
            }
        }
        game *game = initialiseGame(rows, cols, bombs);
        gameLoop(game);
        valid = false;
        while (!valid) {
            printf("Options:\n");
            printf("1. Replay\n");
            printf("2. Quit\n");
            int choice = 0;
            scanf("%d", &choice);
            if (choice != 1 && choice != 2) {
                printf("Invalid option\n");
            } else {
                if (choice == 2) {
                    printf("Thanks for playing\n");
                    freeGame(game);
                    exit(0);
                }
                freeGame(game);
            }
        }
    }
}

int main() {
    srand(time(NULL));
    userMenu();
    return 0;
}