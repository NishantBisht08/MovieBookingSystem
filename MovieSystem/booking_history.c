#include "booking_history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Booking bookingHistory[MAX_BOOKINGS];
int numBookings = 0;

void initBookingHistory() {
    numBookings = 0;
    for (int i = 0; i < MAX_BOOKINGS; i++) {
        bookingHistory[i].numSeats = 0;
        for (int j = 0; j < 10; j++) {
            bookingHistory[i].seatNames[j] = NULL;
        }
    }
    loadBookingHistoryFromFile();
}

void addBooking(char *username, char *movieName, int day, int showtime, Seat *seats, int numSeats, int totalPrice) {
    if (numBookings >= MAX_BOOKINGS) {
        printf("Booking history is full!\n");
        return;
    }

    for (int i = 0; i < numBookings; i++) {
        Booking *temp = &bookingHistory[i];
        if (strcmp(temp->movieName, movieName) == 0 && temp->day == day && temp->showtime == showtime) {
            for (int j = 0; j < temp->numSeats; j++) {
                for (int k = 0; k < numSeats; k++) {
                    char seatName[3];
                    seatName[0] = seats[k].row + 'A';
                    seatName[1] = seats[k].col + '1';
                    seatName[2] = '\0';

                    if (strcmp(temp->seatNames[j], seatName) == 0) {
                        printf("Seat %s is already booked for this showtime!\n", temp->seatNames[j]);
                        return;
                    }
                }
            }
        }
    }

    Booking *booking = &bookingHistory[numBookings];
    strncpy(booking->username, username, MAX_USERNAME - 1);
    booking->username[MAX_USERNAME - 1] = '\0';
    strncpy(booking->movieName, movieName, MAX_MOVIE_NAME - 1);
    booking->movieName[MAX_MOVIE_NAME - 1] = '\0';
    booking->day = day;
    booking->showtime = showtime;
    booking->numSeats = numSeats;
    booking->totalPrice = totalPrice;

    for (int i = 0; i < 10; i++) {
        if (booking->seatNames[i]) {
            free(booking->seatNames[i]);
            booking->seatNames[i] = NULL;
        }
    }

    for (int i = 0; i < numSeats; i++) {
        char seatName[3];
        seatName[0] = seats[i].row + 'A';
        seatName[1] = seats[i].col + '1';
        seatName[2] = '\0';
        booking->seatNames[i] = strdup(seatName);
    }

    numBookings++;
    saveBookingHistoryToFile();
}

void displayBookingHistory(char *username) {
    int found = 0;
    printf("\nBooking History for %s:\n", username);
    for (int i = 0; i < numBookings; i++) {
        Booking *booking = &bookingHistory[i];
        if (strcmp(booking->username, username) == 0) {
            found = 1;
            printf("Booking %d:\n", i + 1);
            printf("  Movie: %s\n", booking->movieName);
            printf("  Day: %d\n", booking->day);
            printf("  Showtime: %02d:%02d\n", booking->showtime / 100, booking->showtime % 100);
            printf("  Seats: ");
            int hasSeats = 0;
            for (int j = 0; j < booking->numSeats; j++) {
                if (booking->seatNames[j]) {
                    printf("%s ", booking->seatNames[j]);
                    hasSeats = 1;
                }
            }
            if (!hasSeats) {
                printf("(none)");
            }
            printf("\n  Total Price: ₹%d\n", booking->totalPrice);
        }
    }
    if (!found) {
        printf("No bookings found for %s.\n", username);
    }
}

int fastCancelBooking(char *username, char *movieName, int day, int showtime, char seatNames[][3], int numSeats) {
    for (int i = 0; i < numBookings; i++) {
        Booking *booking = &bookingHistory[i];
        if (strcmp(booking->username, username) == 0 &&
            strcmp(booking->movieName, movieName) == 0 &&
            booking->day == day &&
            booking->showtime == showtime &&
            booking->numSeats == numSeats) {
            int match = 1;
            char *tempSeats[10];
            for (int j = 0; j < numSeats; j++) {
                tempSeats[j] = strdup(booking->seatNames[j]);
            }
            for (int j = 0; j < numSeats - 1; j++) {
                for (int k = 0; k < numSeats - j - 1; k++) {
                    if (strcmp(tempSeats[k], tempSeats[k + 1]) > 0) {
                        char *temp = tempSeats[k];
                        tempSeats[k] = tempSeats[k + 1];
                        tempSeats[k + 1] = temp;
                    }
                    if (strcmp(seatNames[k], seatNames[k + 1]) > 0) {
                        char tempSeat[3];
                        strcpy(tempSeat, seatNames[k]);
                        strcpy(seatNames[k], seatNames[k + 1]);
                        strcpy(seatNames[k + 1], tempSeat);
                    }
                }
            }
            for (int j = 0; j < numSeats; j++) {
                if (strcmp(tempSeats[j], seatNames[j]) != 0) {
                    match = 0;
                }
                free(tempSeats[j]);
            }
            if (match) {
                for (int j = 0; j < booking->numSeats; j++) {
                    free(booking->seatNames[j]);
                    booking->seatNames[j] = NULL;
                }
                for (int j = i; j < numBookings - 1; j++) {
                    Booking *current = &bookingHistory[j];
                    Booking *next = &bookingHistory[j + 1];
                    strcpy(current->username, next->username);
                    strcpy(current->movieName, next->movieName);
                    current->day = next->day;
                    current->showtime = next->showtime;
                    current->numSeats = next->numSeats;
                    current->totalPrice = next->totalPrice;
                    for (int k = 0; k < 10; k++) {
                        if (current->seatNames[k]) {
                            free(current->seatNames[k]);
                            current->seatNames[k] = NULL;
                        }
                        current->seatNames[k] = next->seatNames[k] ? strdup(next->seatNames[k]) : NULL;
                    }
                }
                numBookings--;
                Booking *last = &bookingHistory[numBookings];
                for (int j = 0; j < 10; j++) {
                    if (last->seatNames[j]) {
                        free(last->seatNames[j]);
                        last->seatNames[j] = NULL;
                    }
                }
                last->numSeats = 0;
                saveBookingHistoryToFile();
                return 1;
            }
        }
    }
    return 0;
}

void saveBookingHistoryToFile() {
    FILE *file = fopen("booking_history.txt", "w");
    if (!file) {
        printf("Error opening booking_history.txt for writing.\n");
        return;
    }

    for (int i = 0; i < numBookings; i++) {
        Booking *booking = &bookingHistory[i];
        fprintf(file, "%s,%s,%d,%d,%d,%d", booking->username, booking->movieName, booking->day, booking->showtime, booking->numSeats, booking->totalPrice);
        for (int j = 0; j < booking->numSeats; j++) {
            fprintf(file, ",%s", booking->seatNames[j] ? booking->seatNames[j] : "NONE");
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void loadBookingHistoryFromFile() {
    FILE *file = fopen("booking_history.txt", "r");
    if (!file) {
        return;
    }

    numBookings = 0;
    char line[512];
    while (fgets(line, sizeof(line), file) && numBookings < MAX_BOOKINGS) {
        Booking *booking = &bookingHistory[numBookings];
        char *token = strtok(line, ",");
        strncpy(booking->username, token, MAX_USERNAME - 1);
        booking->username[MAX_USERNAME - 1] = '\0';

        token = strtok(NULL, ",");
        strncpy(booking->movieName, token, MAX_MOVIE_NAME - 1);
        booking->movieName[MAX_MOVIE_NAME - 1] = '\0';

        token = strtok(NULL, ",");
        booking->day = atoi(token);

        token = strtok(NULL, ",");
        booking->showtime = atoi(token);

        token = strtok(NULL, ",");
        booking->numSeats = atoi(token);

        token = strtok(NULL, ",");
        booking->totalPrice = atoi(token);

        for (int i = 0; i < booking->numSeats; i++) {
            token = strtok(NULL, ",");
            char *seatName = strtok(token, "\n");
            booking->seatNames[i] = (seatName && strcmp(seatName, "NONE") != 0) ? strdup(seatName) : NULL;
        }

        numBookings++;
    }

    fclose(file);
}

void resetBookingHistory() {
    // Free all seat names
    for (int i = 0; i < numBookings; i++) {
        Booking *booking = &bookingHistory[i];
        for (int j = 0; j < booking->numSeats; j++) {
            if (booking->seatNames[j]) {
                free(booking->seatNames[j]);
                booking->seatNames[j] = NULL;
            }
        }
        booking->numSeats = 0;
    }
    numBookings = 0;

    // Overwrite booking_history.txt with an empty file
    FILE *file = fopen("booking_history.txt", "w");
    if (!file) {
        printf("Error opening booking_history.txt for resetting.\n");
        return;
    }
    fclose(file);
}