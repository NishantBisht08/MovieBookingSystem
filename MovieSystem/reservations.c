#include "reservations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int isSeatAvailable(char *movieName, int day, int showtime, int row, int col) {
    Movie *movie = searchMovie(movieName);
    if (!movie || movie->day != day) return 0;
    int showtimeIdx = -1;
    for (int i = 0; i < NUM_SHOWTIMES; i++) {
        if (movie->showtimes[i] == showtime) {
            showtimeIdx = i;
            break;
        }
    }
    if (showtimeIdx == -1 || row < 0 || row >= ROWS || col < 0 || col >= COLS) return 0;
    return movie->seatMatrix[showtimeIdx][row][col] == 0;
}

int bookSeat(char *movieName, int day, int showtime, int row, int col) {
    Movie *movie = searchMovie(movieName);
    if (!movie || movie->day != day) return 0;
    int showtimeIdx = -1;
    for (int i = 0; i < NUM_SHOWTIMES; i++) {
        if (movie->showtimes[i] == showtime) {
            showtimeIdx = i;
            break;
        }
    }
    if (showtimeIdx == -1 || row < 0 || row >= ROWS || col < 0 || col >= COLS) return 0;
    if (movie->seatMatrix[showtimeIdx][row][col] == 0) {
        movie->seatMatrix[showtimeIdx][row][col] = 1;
        saveSeatMatrices();
        return 1;
    }
    return 0;
}

int cancelSeat(char *movieName, int day, int showtime, int row, int col) {
    Movie *movie = searchMovie(movieName);
    if (!movie || movie->day != day) return 0;
    int showtimeIdx = -1;
    for (int i = 0; i < NUM_SHOWTIMES; i++) {
        if (movie->showtimes[i] == showtime) {
            showtimeIdx = i;
            break;
        }
    }
    if (showtimeIdx == -1 || row < 0 || row >= ROWS || col < 0 || col >= COLS) return 0;
    if (movie->seatMatrix[showtimeIdx][row][col] == 1) {
        movie->seatMatrix[showtimeIdx][row][col] = 0;
        saveSeatMatrices();
        return 1;
    }
    return 0;
}

void displaySeats(char *movieName, int day, int showtime) {
    Movie *movie = searchMovie(movieName);
    if (!movie || movie->day != day) {
        printf("Movie not found!\n");
        return;
    }
    int showtimeIdx = -1;
    for (int i = 0; i < NUM_SHOWTIMES; i++) {
        if (movie->showtimes[i] == showtime) {
            showtimeIdx = i;
            break;
        }
    }
    if (showtimeIdx == -1) {
        printf("Showtime not found!\n");
        return;
    }

    printf("\nSeat Matrix for %s at %02d:%02d:\n", movieName, showtime / 100, showtime % 100);
    printf("   1 2 3 4 5\n");
    for (int i = 0; i < ROWS; i++) {
        printf("%c  ", 'A' + i);
        for (int j = 0; j < COLS; j++) {
            printf("%d ", movie->seatMatrix[showtimeIdx][i][j]);
        }
        printf("\n");
    }
}

Seat* bookSeatsBFS(char *movieName, int day, int showtime, int startRow, int startCol, int totalSeats) {
    Seat *seats = (Seat *)malloc(totalSeats * sizeof(Seat));
    if (!seats) return NULL;

    int booked = 0;
    seats[booked++] = (Seat){startRow, startCol};
    bookSeat(movieName, day, showtime, startRow, startCol);

    int queue[ROWS * COLS][2], front = 0, rear = 0;
    int visited[ROWS][COLS] = {0};
    queue[rear][0] = startRow;
    queue[rear][1] = startCol;
    rear++;
    visited[startRow][startCol] = 1;

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    while (front < rear && booked < totalSeats) {
        int r = queue[front][0];
        int c = queue[front][1];
        front++;

        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && !visited[nr][nc] && isSeatAvailable(movieName, day, showtime, nr, nc)) {
                seats[booked++] = (Seat){nr, nc};
                bookSeat(movieName, day, showtime, nr, nc);
                queue[rear][0] = nr;
                queue[rear][1] = nc;
                rear++;
                visited[nr][nc] = 1;
                if (booked == totalSeats) break;
            }
        }
    }

    if (booked < totalSeats) {
        for (int i = 0; i < booked; i++) {
            cancelSeat(movieName, day, showtime, seats[i].row, seats[i].col);
        }
        free(seats);
        return NULL;
    }
    return seats;
}

Seat* bookSeatsGreedy(char *movieName, int day, int showtime, int startRow, int startCol, int totalSeats) {
    Seat *seats = (Seat *)malloc(totalSeats * sizeof(Seat));
    if (!seats) return NULL;

    int booked = 0;
    for (int c = startCol; c < COLS && booked < totalSeats; c++) {
        if (isSeatAvailable(movieName, day, showtime, startRow, c)) {
            seats[booked++] = (Seat){startRow, c};
            bookSeat(movieName, day, showtime, startRow, c);
        }
    }

    if (booked < totalSeats) {
        for (int i = 0; i < booked; i++) {
            cancelSeat(movieName, day, showtime, seats[i].row, seats[i].col);
        }
        free(seats);
        return NULL;
    }
    return seats;
}

Seat* bookSeatsCenterGreedy(char *movieName, int day, int showtime, int totalSeats) {
    Seat *seats = (Seat *)malloc(totalSeats * sizeof(Seat));
    if (!seats) return NULL;

    int booked = 0;
    int centerCol = COLS / 2; // Center column (2 for 0-4)
    for (int dist = 0; dist <= centerCol && booked < totalSeats; dist++) {
        for (int c = centerCol - dist; c <= centerCol + dist && booked < totalSeats; c++) {
            if (c < 0 || c >= COLS) continue;
            for (int r = 0; r < ROWS && booked < totalSeats; r++) {
                if (isSeatAvailable(movieName, day, showtime, r, c)) {
                    seats[booked++] = (Seat){r, c};
                    bookSeat(movieName, day, showtime, r, c);
                }
            }
        }
    }

    if (booked < totalSeats) {
        for (int i = 0; i < booked; i++) {
            cancelSeat(movieName, day, showtime, seats[i].row, seats[i].col);
        }
        free(seats);
        return NULL;
    }
    return seats;
}