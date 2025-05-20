#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#include "movie_list.h"

#define ROWS 5
#define COLS 5

typedef struct {
    int row;
    int col;
} Seat;

int isSeatAvailable(char *movieName, int day, int showtime, int row, int col);
int bookSeat(char *movieName, int day, int showtime, int row, int col);
int cancelSeat(char *movieName, int day, int showtime, int row, int col);
void displaySeats(char *movieName, int day, int showtime);
Seat* bookSeatsBFS(char *movieName, int day, int showtime, int startRow, int startCol, int totalSeats);
Seat* bookSeatsGreedy(char *movieName, int day, int showtime, int startRow, int startCol, int totalSeats);
Seat* bookSeatsCenterGreedy(char *movieName, int day, int showtime, int totalSeats);

#endif