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
int sum[NUM_CHILD_THREADS];
int sudokuPuzzle[9][9];
int tid_columns[9]; /* thread IDs for columns, rows, and subgrids */
int tid_rows[9];
int tid_subgrids[9];
bool columns[9]; /* boolean arrays for columns, rows, and subgrids */
bool rows[9];
bool subgrids[9];

void *runner(void *param); /* the thread */

/* represents the index (row and column) range */
typedef struct {
    int topRow;
    int bottomRow;
    int leftColumn;
    int rightColumn;
} run_param;

/* checks the columns */
/*
void *checkColumn (void* param) {

}
*/

/* checks the rows */
/*
void *checkRow (void* param) {

}
*/

/* checks subgrids 3x3 */
/*
void *checkSubGrid (void* param) {

}
*/


int main() {
    /* opens up .txt file */
    FILE *fp = fopen("SudokuPuzzle.txt", "r");
    if (fp == NULL) {
        printf("Error. Could not open file");
        /* Program exits if the file pointer returns NULL. */
        exit(1);
    } else {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                int num;
                if (fscanf(fp, " %d", &num) != 1)
                    printf("Error reading input file");
                else
                    sudokuPuzzle[i][j] = num;
            }
        }
    }
    fclose(fp);

    /* print outs the sudoku puzzle */
    printf("\n");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("%d\t", sudokuPuzzle[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
