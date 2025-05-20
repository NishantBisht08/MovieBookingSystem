#ifndef MOVIE_LIST_H
#define MOVIE_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10
#define NUM_SHOWTIMES 5
#define ROWS 5
#define COLS 5

typedef struct Movie {
    char name[50];
    char genre[20];
    int day;                    // 1=Monday, 2=Tuesday, etc.
    int showtimes[NUM_SHOWTIMES]; // e.g., {1430, 1700, ...}
    int seatMatrix[NUM_SHOWTIMES][ROWS][COLS]; // 5x5 matrix per showtime
    struct Movie *next;
} Movie;

extern Movie *movieTable[TABLE_SIZE];

void initMovieTable();
void insertMovie(char *name, char *genre, int day, int showtimes[], int seatMatrix[][ROWS][COLS]);
Movie* searchMovie(char *name);
void displayMoviesForDay(int day);
void displayShowtimes(char *movieName, int day);
void loadSeatMatrices();
void saveSeatMatrices();
void resetSeatMatrices(); // New function to reset seat matrices

#endif