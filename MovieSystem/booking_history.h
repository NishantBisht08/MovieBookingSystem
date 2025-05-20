#ifndef BOOKING_HISTORY_H
#define BOOKING_HISTORY_H

#include "movie_list.h"
#include "reservations.h"

#define MAX_BOOKINGS 100
#define MAX_USERNAME 50
#define MAX_MOVIE_NAME 50

typedef struct {
    char username[MAX_USERNAME];
    char movieName[MAX_MOVIE_NAME];
    int day;
    int showtime;
    char *seatNames[10];
    int numSeats;
    int totalPrice;
} Booking;

extern Booking bookingHistory[MAX_BOOKINGS];
extern int numBookings;

void initBookingHistory();
void loadBookingHistoryFromFile();
void saveBookingHistoryToFile();
void addBooking(char *username, char *movieName, int day, int showtime, Seat *seats, int numSeats, int totalPrice);
void displayBookingHistory(char *username);
int fastCancelBooking(char *username, char *movieName, int day, int showtime, char seatNames[][3], int numSeats);
void resetBookingHistory(); // New function to reset booking history

#endif