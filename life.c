/***************************************************************************
 * life.c
 *
 * MPATE-GE 2618
 * Problem Set 3
 *
 * Implements Conway's Game of Life
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "inputlib.h"

/* constants */
#define NUM_ROWS 25
#define NUM_COLS 70
#define ALIVE_CHAR '*'
#define DEAD_CHAR '.'
#define FRAME_RATE_DEFAULT 500000

/* global grid */
bool grid[NUM_ROWS][NUM_COLS];

/* frame rate */
float frameRateSetting = 1.0;

/* toroidal space or not */
bool wrap = false;

// function prototypes
void clear();
void init();
void drawGrid();
void drawMenu();
void step();
void executeChoice(int);
void openFile(const char *fileName);
void saveFile(const char *fileName);
void randomizeGrid();
int counter_Alive(int j, int i);
int neighbourGrid(int j, int i);

int main(int argc, char * argv[])
{
    /* check to make sure there are either zero or one argument */
    if (argc > 2) {
        printf("Usage: %s [t]\n", argv[0]);
        return 1;
    }
    /* if an argument is supplied... */
    if (argc == 2) {
        if (strcmp("t", argv[1]) != 0) {
            printf("Usage: %s [t]\n", argv[0]);
            return 1;
        }
        /* if argument 't' present, set the grid in wrap mode */
        wrap = true;
    }

    // initialize the grid
    init();

    while (true) {
        clear();
        drawGrid();
        drawMenu();
    }
}

/* Clears screen using ANSI escape sequences. */
void clear()
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/* Prints the menu and prompts the user to choose menu item. */
void drawMenu()
{
    int choice;

    printf("[1] Simulate\n" \
            "[2] Read file\n" \
            "[3] Save file\n" \
            "[4] Randomize grid\n" \
            "[5] Change frame rate\n" \
            "[6] Quit\n" \
            "SELECT A CHOICE BY NUMBER: ");
    choice = getInt();
    while (choice < 1 || choice > 6) {
        printf("Bad selection. Try again: ");
        choice = getInt();
    }
    executeChoice(choice);
}

/* Given the user's menu choice, takes action */
void executeChoice(int choice)
{
    int numSteps, i; 
    char *filename;
    float newFrameRate; 

    switch (choice) {
        case 1:    // Start simulation. Ask user how many steps to simulate
            printf("How many steps to simulate? ");
            numSteps = getInt();
            while (numSteps < 0) {
                printf("Enter positive value. Try again: ");
                numSteps = getInt();
            }
            for (i = 0; i < numSteps; i++) {
                step();
                clear();
                drawGrid();
                // sleep thread for animation's sake
                usleep(frameRateSetting * FRAME_RATE_DEFAULT);
            }
            break;
        case 2:      // Read in a file
            printf("Enter filename to read: ");
            filename = getString();
            openFile(filename);
            free (filename);
            break;
        case 3:     // Save the current environment state to a file
            printf("Enter filename to write: ");
            filename = getString();
            saveFile(filename);
            free (filename);

            break;
        case 4:     // Generate a random environment
            randomizeGrid();
            break;
        case 5:     // Change speed of animation
            printf("Current speed is %.2f times default. Enter new multiplier: ", frameRateSetting);
            newFrameRate= getFloat();
            while (newFrameRate < 0.0) {
                printf("Enter positive value. Try again: ");
                newFrameRate = getFloat();
            }
            frameRateSetting = newFrameRate;
            printf("Frame rate changed.\n");
            usleep(1000000);
            break;
        case 6: // Quit
            printf("Goodbye!\n");
            exit(0);
    }
}

/* Initializes the global grid with an empty environment.,
 * i.e., fills 2D array with all false values. */
void init()
{ 
    int i, j;

    for (j=0; j< NUM_ROWS; j++){
        for (i=0; i < NUM_COLS; i++){
            grid[j][i] = false; 

        }
    }
}

/* Prints the grid in its current state. 
 * Live cells are represented by ALIVE_CHARs and dead cells by DEAD_CHARs. 
 * It's up to you how to frame the grid to make it look nicer */
void drawGrid()
{
    int i, j;

    for (j = 0; j < NUM_ROWS; j++) {
        for (i = 0; i < NUM_COLS; i++) {
            if (grid[j][i] == false){
                printf("%c", DEAD_CHAR);
            }

            else {
                printf ("%c", ALIVE_CHAR);
            }
        }
        printf("\n");

    }
}

/* Read in a file with an preset environment.  Watch out for the return character 
 * at the end of each line in the file*/    
void openFile(const char *filename)
{
  FILE *fp;
  char ch;
  int i, j;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("File not found.\n");
    usleep(1000000);
    return;
  }

  for (j = 0; j < NUM_ROWS; j++) {
    for (i = 0; i < NUM_COLS; i++) {
      ch = fgetc(fp);
      if (ch==EOF) {
        printf("BAD FILE.\n");
        if (ch==EOF) {
          printf("EOF reached\n");
        }
        return;
      }
      
      if (ch == '0')
        grid[j][i] = false;
      else
        grid[j][i] = true;
      
    }
    ch = fgetc(fp); // for return character
  }
  fclose(fp);
}
/* Save the current environment to a file in the .lif file format.
 * The format has NUM_ROWS of '0' or '1' characters. Each row is
 * NUM_COLS in length and followed by a return (no spaces).  A '0'
 * represents a dead cell and a '1' represents a live cell.
 * See .lif file in the lifeExtras/ directory for examples of the format.
 */
void saveFile(const char *filename) 
{
    FILE *outfp;
    int i, j;

    // open the file for writing 
    outfp = fopen(filename, "w");



    for (j = 0; j< NUM_ROWS; j++) {
        for (i = 0; i < NUM_COLS; i++){
            if (grid[j][i] == false){
                putc('0', outfp);// Changes dead characters into 0 in saved file
            }
            else {
                putc('1', outfp);
            }
            putc('\n', outfp); 
        }
    }
    // close file handles 
    fclose(outfp);
    printf("File saved.\n");
}

/* Transition to the next generation based on the following rules:
 *  1. Any live cell with fewer than two live neighbors dies
 *  2. Any live cell with more than three live neighbors dies
 *  3. Any live cell with two or three live neighbors lives, unchanged
 *  4. Any dead cell with exactly three live neighbors comes to life
 * Since the grid is not infinite, you must consider any neighbors out of 
 * bounds to be "dead." 
 */
void step()
{
    int i = 0, j = 0; 
    bool tempgrid[NUM_ROWS][NUM_COLS];

    for (j = 0; j < NUM_ROWS; j++) {
        for (i = 0; i < NUM_COLS; i++){
            tempgrid[j][i] = false;
        }
    }
    for (j = 0; j < NUM_ROWS; j++) {
        for (i = 0; i < NUM_COLS; i++){

      int aliveNeighbours = counter_Alive(j, i);

      //rule 4
      if (aliveNeighbours == 3 && grid[j][i] == false){ 
          tempgrid[j][i] = true;
      } 

      //rule 1
      if (aliveNeighbours < 2 && grid[j][i] == true){ 
        tempgrid[j][i] = false;
      }

      //rule 2
      if (aliveNeighbours > 3 && grid[j][i] == true){ 
        tempgrid[j][i] = false;
      }

      //rule 3
      if( (aliveNeighbours == 2 || aliveNeighbours == 3) && grid[j][i] == true){
        tempgrid[j][i] = true;
      }

    }
  }

  for (j = 0; j < NUM_ROWS; j++) {
    for (i = 0; i < NUM_COLS; i++){ 
      grid[j][i] = tempgrid[j][i];
    }
  }
}

//Creating a counter to determine where in the grid there is an alive character
int counter_Alive (int j, int i){

    bool isLeftAlive = neighborGrid(j, i - 1);
    bool isRightAlive = neighborGrid(j, i + 1);
    bool isUpAlive = neighborGrid(j - 1, i);
    bool isDownAlive = neighborGrid(j + 1, i);
    bool isUpLeftAlive = neighborGrid (j - 1, i - 1);
    bool isUpRightAlive = neighborGrid (j - 1, i + 1);
    bool isDownLeftAlive = neighborGrid (j + 1, i - 1);
    bool isDownRightAlive = neighborGrid (j + 1, i + 1);

    int aliveNeighbours = 0;

      if (isLeftAlive == true){
        aliveNeighbours++;
      }

      if (isRightAlive == true){
        aliveNeighbours++;
      }

      if (isUpAlive == true){
        aliveNeighbours++;
      }

      if (isDownAlive == true){
        aliveNeighbours++;
      }

      if (isUpLeftAlive == true){
        aliveNeighbours++;
      }

      if (isUpRightAlive == true){
        aliveNeighbours++;
      }

      if (isDownLeftAlive == true){
        aliveNeighbours++;
      }

      if (isDownRightAlive == true){
        aliveNeighbours++;
      }

      return aliveNeighbours;
}

//Make sure alive or dead characters are not out of bounds. Only 
//check where neighbor should be existing if not return false.

  int neighborGrid (int j, int i){
    if (j < 0 || j > NUM_ROWS){
        return false;
    }

    if (i < 0 || i > NUM_COLS){
        return false;
    }

    return grid [j][i]; 
}

/* Create a randomly generated environment. 
 * For each position in the grid randomly generate
 * either a live or dead cell. */ 
void randomizeGrid()
{ 
    int i, j;
    srand (time(NULL));

    for (j=0; j< NUM_ROWS; j++){
        for (i=0; i < NUM_COLS; i++){
            grid [j][i] = rand() % 2;
            }
        }
    }



