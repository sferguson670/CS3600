/**
 * A program that will take a .txt input and will determine
 * whether or not the Sudoku puzzle is valid.
 * Uses multi-threading.
 *
 * Sarah Ferguson
 * Sept. 2018
 * CS 3600
 * HW #2
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_CHILD_THREADS 27
#define TRUE 1
#define FALSE 0

typedef int bool;

/* global variables */
int sudokuPuzzle[9][9]; /* matrix to store the puzzle */
int tid_columns[9]; /* thread IDs for columns, rows, and subgrids */
int tid_rows[9];
int tid_subgrids[9];
bool columns[9]; /* boolean arrays for columns, rows, and subgrids */
bool rows[9];
bool subgrids[9];

/* represents the index (row and column) range */
typedef struct {
    int topRow;
    int bottomRow;
    int leftColumn;
    int rightColumn;
} run_param;

/* checks the columns */
void *checkColumn (void* param) {
    pthread_t self;
    run_param *params = (run_param*) param;
    int row = params -> topRow;
    int column = params -> rightColumn;

    if (row != 0 || column > 8){
        fprintf(stderr, "\nInvalid. Row: %d Column: %d", row, column);
        pthread_exit(NULL);
    }
    self = pthread_self();
    int validColumns[9] = {0};
    int i;
    for (i = 0; i < 9; i++) {
        int num = sudokuPuzzle[i][column];
        if (num < 1 || num > 9 || validColumns[num - 1] == 1) {
            tid_columns[column] = self;
            columns[column] = FALSE;
            pthread_exit(NULL);
        } else {
            validColumns[num - 1] = 1;
        }
    }
    tid_columns[column] = self;
    columns[column] = TRUE;
    pthread_exit(NULL);
}

/* checks the rows */
void *checkRow (void* param) {
    pthread_t self;
    run_param *params = (run_param*) param;
    int row = params -> topRow;
    int column = params -> leftColumn;

    if (column!= 0 || row > 8){
        fprintf(stderr, "\nInvalid. Row: %d Column: %d", row, column);
        pthread_exit(NULL);
    }
    self = pthread_self();
    int validRows[9] = {0};

    int i;
    for (i = 0; i < 9; i++) {
        int num = sudokuPuzzle[row][i];
        if (num < 1 || num > 9 || validRows[num - 1] == 1) {
            tid_rows[row] = self;
            rows[row] = FALSE;
            pthread_exit(NULL);
        } else {
            validRows[num - 1] = 1;
        }
    }
    tid_rows[row] = self;
    rows[row] = TRUE;
    pthread_exit(NULL);
}

/* checks subgrids 3x3 */
void *checkSubGrid (void* param) {
    pthread_t self;
    run_param *params = (run_param*) param;
    int row = params -> topRow;
    int column = params -> rightColumn;

    if (row > 6 || row % 3 != 0 || column > 6 || column % 3 != 0){
        fprintf(stderr, "\nInvalid. Row: %d Column: %d", row, column);
        pthread_exit(NULL);
    }
    self = pthread_self();
    int validSubGrid[9] = {0};

    int i, j;
    for (i = row; i < 9; i++) {
        for (j = column; j < column + 3; j++) {
            int num = sudokuPuzzle[i][j];
            if (num < 1 || num > 9 || validSubGrid[num-1] == 1) {
                tid_subgrids[row + column / 3] = self;
                subgrids[row + column / 3] = FALSE;
                pthread_exit(NULL);
            }
            else {
                validSubGrid[num - 1] = 1;
            }
        }
    }
    tid_subgrids[row + column / 3] = self;
    tid_subgrids[row + column / 3] = TRUE;
    pthread_exit(NULL);
}

int main() {
    int threadIndex = 0;
    pthread_t tid[NUM_CHILD_THREADS]; /* thread identifier */

    /* opens up .txt file */
    FILE *fp = fopen("SudokuPuzzle.txt", "r");
    if (fp == NULL) {
        printf("\nError. Could not open file");
        /* Program exits if the file pointer returns NULL. */
        exit(1);
    } else {
        int i, j;
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                int num;
                if (fscanf(fp, " %d", &num) != 1)
                    printf("\nError reading input file");
                else
                    sudokuPuzzle[i][j] = num;
            }
        }
    }
    fclose(fp);
-
    /* print outs the sudoku puzzle */
    printf("\nSudoku Puzzle: \n");
    int i, j;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            printf("%d\t", sudokuPuzzle[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    /* creates 9 threads to check columns */
    for (int i = 0; i < 9; i++) {
        run_param *position = (run_param*) malloc(sizeof(run_param));
        position -> topRow = 0;
        position -> bottomRow = 8;
        position -> leftColumn = i;
        position -> rightColumn = i;
        pthread_create(&tid[threadIndex++], NULL, checkColumn, position);
    }

    /* creates 9 threads to check rows */
    for (int i = 0; i < 9; i++) {
        run_param *position = (run_param*) malloc(sizeof(run_param));
        position -> topRow = i;
        position -> bottomRow = i;
        position -> leftColumn = 0;
        position -> rightColumn = 8;
        pthread_create(&tid[threadIndex++], NULL, checkRow, position);
    }

    /* creates 9 threads to check subgrids */
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (i % 3 == 0 && j % 3 == 0) {
                run_param *position = (run_param*) malloc(sizeof(run_param));
                position -> topRow = i;
                position -> bottomRow = i + 3;
                position -> rightColumn = j;
                position -> leftColumn = j + 3;
                pthread_create(&tid[threadIndex++], NULL, checkSubGrid, position);
            }
        }
    }

    /* join threads and wait for them to exit */
    for (int i = 0; i < NUM_CHILD_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    /* print out output */
    printf("\n\nResults-------------------\n");
    int check = TRUE;

    for (int i = 0; i < 9; i++) {
        /* checks columns */
        if (columns[i] == TRUE)
            printf("\nColumn: %1X is valid.", (unsigned int)tid_columns);
        else {
            check = FALSE;
            printf("\nColumn: %1X is invalid.", (unsigned int)tid_columns);
        }
        /* checks rows */
        if (rows[i] == TRUE)
            printf("\nRow: %1X is valid.", (unsigned int)tid_rows);
        else {
            check = FALSE;
            printf("\nRow: %1X is invalid.", (unsigned int)tid_rows);
        }
        /* checks subgrids */
        if (subgrids[i] == TRUE)
            printf("\nSubgrid: %1X is valid.", (unsigned int)tid_subgrids);
        else {
            check = FALSE;
            printf("\nSubgrid: %1X is invalid.", (unsigned int)tid_subgrids);
        }
    }
    if (check == TRUE)
        printf("\nSudoku puzzle is correct.\n");
    else
        printf("\nSudoku puzzle is incorrect.\n");

    return EXIT_SUCCESS;
}
