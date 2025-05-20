#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movie_list.h"
#include "reservations.h"
#include "booking_history.h"

void displayMenu() {
    printf("\n===== Movie Ticket Booking System =====\n");
    printf("1. Display Movies\n");
    printf("2. Book Seats\n");
    printf("3. View Booking History\n");
    printf("4. Cancel Booking\n");
    printf("5. Exit\n");
    printf("6. Reset System\n");
    printf("Enter your choice (1-6, or 7 to exit): ");
}

int getNumberInput(int min, int max) {
    int num;
    while (scanf("%d", &num) !=+ 1 || num < min || (num > max && num != 7)) {
        printf("Invalid input! Please enter a number between %d and %d (or 7 to exit): ", min, max);
        while (getchar() != '\n');
    }
    while (getchar() != '\n'); // Ensure the buffer is fully cleared
    return num;
}

void cleanupBookingHistory() {
    for (int i = 0; i < numBookings; i++) {
        for (int j = 0; j < bookingHistory[i].numSeats; j++) {
            if (bookingHistory[i].seatNames[j]) {
                free(bookingHistory[i].seatNames[j]);
                bookingHistory[i].seatNames[j] = NULL;
            }
        }
    }
}

int main() {
    initMovieTable();
    initBookingHistory();
    loadBookingHistoryFromFile();

    while (1) {
        printf("\nSelect a day:\n");
        printf("1. Monday\n2. Tuesday\n3. Wednesday\n4. Thursday\n5. Friday\n");
        printf("Enter day (1-5, or 7 to exit): ");
        int day = getNumberInput(1, 5);
        if (day == 7) {
            saveBookingHistoryToFile();
            saveSeatMatrices();
            cleanupBookingHistory();
            printf("Exiting program...\n");
            return 0;
        }

        printf("Enter your name: ");
        char username[50];
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = 0;

        int choice;
        do {
            displayMenu();
            choice = getNumberInput(1, 6);
            if (choice == 7) break;

            switch (choice) {
                case 1: // Display Movies
                    displayMoviesForDay(day);
                    break;

                case 2: { // Book Seats
                    displayMoviesForDay(day);
                    printf("Select a movie number (or 7 to exit): ");
                    int movieChoice = getNumberInput(1, 1);
                    if (movieChoice == 7) break;

                    char *movieName = NULL;
                    for (int i = 0; i < TABLE_SIZE; i++) {
                        Movie *temp = movieTable[i];
                        while (temp) {
                            if (temp->day == day) {
                                movieName = temp->name;
                                break;
                            }
                            temp = temp->next;
                        }
                        if (movieName) break;
                    }
                    if (!movieName) {
                        printf("No movie found for this day!\n");
                        break;
                    }

                    displayShowtimes(movieName, day);
                    printf("Select a showtime (1-5, or 7 to exit): ");
                    int showtimeChoice = getNumberInput(1, NUM_SHOWTIMES);
                    if (showtimeChoice == 7) break;

                    int showtime = 0;
                    Movie *movie = searchMovie(movieName);
                    if (movie) showtime = movie->showtimes[showtimeChoice - 1];

                    displaySeats(movieName, day, showtime);
                    int prices[ROWS] = {400, 350, 300, 250, 200};
                    printf("Prices per row:\n");
                    for (int i = 0; i < ROWS; i++) {
                        printf("Row %c: ₹%d\n", 'A' + i, prices[i]);
                    }

                    printf("Enter total seats to book (1-10, or 7 to exit): ");
                    int totalSeats = getNumberInput(1, 10);
                    if (totalSeats == 7) break;

                    printf("Choose booking method:\n");
                    printf("1. Manual\n2. BFS\n3. Greedy (Left-to-Right)\n4. Center-Greedy\n");
                    printf("Enter choice (1-4, or 7 to exit): ");
                    int method = getNumberInput(1, 4);
                    if (method == 7) break;

                    Seat *seats = NULL;
                    if (method == 1) {
                        seats = (Seat *)malloc(totalSeats * sizeof(Seat));
                        for (int i = 0; i < totalSeats; i++) {
                            char seat[3];
                            int valid = 0;
                            while (!valid) {
                                printf("Enter seat %d (e.g., A1, or 7 to exit): ", i + 1);
                                fgets(seat, sizeof(seat), stdin);
                                seat[strcspn(seat, "\n")] = 0;
                                if (strcmp(seat, "7") == 0) {
                                    free(seats);
                                    seats = NULL;
                                    break;
                                }
                                if (strlen(seat) == 2 && seat[0] >= 'A' && seat[0] <= 'E' && seat[1] >= '1' && seat[1] <= '5') {
                                    int row = seat[0] - 'A';
                                    int col = seat[1] - '1';
                                    if (isSeatAvailable(movieName, day, showtime, row, col)) {
                                        seats[i] = (Seat){row, col};
                                        bookSeat(movieName, day, showtime, row, col);
                                        valid = 1;
                                    } else {
                                        printf("Seat is booked! Try again.\n");
                                    }
                                } else {
                                    printf("Invalid seat format! Enter like A1.\n");
                                }
                            }
                            if (!valid) break;
                        }
                    } else {
                        if (method != 4) {
                            char seat[3];
                            printf("Enter starting seat (e.g., A1, or 7 to exit): ");
                            fgets(seat, sizeof(seat), stdin);
                            seat[strcspn(seat, "\n")] = 0;
                            if (strcmp(seat, "7") == 0) break;
                            if (strlen(seat) != 2 || seat[0] < 'A' || seat[0] > 'E' || seat[1] < '1' || seat[1] > '5') {
                                printf("Invalid seat format! Enter like A1.\n");
                                break;
                            }
                            int startRow = seat[0] - 'A';
                            int startCol = seat[1] - '1';
                            if (!isSeatAvailable(movieName, day, showtime, startRow, startCol)) {
                                printf("Seat is booked! Try again.\n");
                                break;
                            }
                            if (method == 2) {
                                seats = bookSeatsBFS(movieName, day, showtime, startRow, startCol, totalSeats);
                            } else {
                                seats = bookSeatsGreedy(movieName, day, showtime, startRow, startCol, totalSeats);
                            }
                        } else {
                            seats = bookSeatsCenterGreedy(movieName, day, showtime, totalSeats);
                        }
                        if (!seats) {
                            printf("Not enough available seats!\n");
                            break;
                        }
                    }

                    if (seats) {
                        int totalPrice = 0;
                        for (int i = 0; i < totalSeats; i++) {
                            totalPrice += prices[seats[i].row];
                        }
                        printf("Total Price: ₹%d\n", totalPrice);
                        printf("Pay now? (yes/no, or 7 to exit): ");
                        char payment[10];
                        int validPayment = 0;
                        while (!validPayment) {
                            fgets(payment, sizeof(payment), stdin);
                            payment[strcspn(payment, "\n")] = 0;
                            if (strcmp(payment, "7") == 0) break;
                            if (strcasecmp(payment, "yes") == 0 || strcasecmp(payment, "no") == 0) {
                                validPayment = 1;
                            } else {
                                printf("Invalid input! Enter yes or no (or 7 to exit): ");
                            }
                        }
                        if (strcmp(payment, "7") == 0) {
                            for (int i = 0; i < totalSeats; i++) {
                                cancelSeat(movieName, day, showtime, seats[i].row, seats[i].col);
                            }
                            free(seats);
                            break;
                        }
                        if (strcasecmp(payment, "yes") == 0) {
                            addBooking(username, movieName, day, showtime, seats, totalSeats, totalPrice);
                            printf("Seats booked successfully!\n");
                            printf("User: %s, Day: %d, Movie: %s, Showtime: %02d:%02d, Seats: ", username, day,
                                   movieName, showtime / 100, showtime % 100);
                            for (int i = 0; i < totalSeats; i++) {
                                printf("%c%d ", 'A' + seats[i].row, seats[i].col + 1);
                            }
                            printf("\n");
                        } else {
                            for (int i = 0; i < totalSeats; i++) {
                                cancelSeat(movieName, day, showtime, seats[i].row, seats[i].col);
                            }
                        }
                        free(seats);
                    }
                    break;
                }

                case 3:
                    printf("Enter Username: ");
                    fgets(username, sizeof(username), stdin);
                    username[strcspn(username, "\n")] = 0;
                    displayBookingHistory(username);
                    break;

                case 4: {
                    printf("Enter Username: ");
                    fgets(username, sizeof(username), stdin);
                    username[strcspn(username, "\n")] = 0;

                    displayBookingHistory(username);
                    printf("Enter Movie Name: ");
                    char movieName[50];
                    fgets(movieName, sizeof(movieName), stdin);
                    movieName[strcspn(movieName, "\n")] = 0;

                    printf("Enter Day (1-5, or 7 to exit): ");
                    int cancelDay = getNumberInput(1, 5);
                    if (cancelDay == 7) break;

                    printf("Enter Showtime (e.g., 1430 for 14:30, or 7 to exit): ");
                    int cancelShowtime = getNumberInput(1, 2359); // Use getNumberInput for consistency
                    if (cancelShowtime == 7) break;

                    printf("Enter number of seats to cancel: ");
                    int numSeats = getNumberInput(1, 10);
                    if (numSeats == 7) break;

                    char seatNames[10][3];
                    for (int i = 0; i < numSeats; i++) {
                        int valid = 0;
                        while (!valid) {
                            printf("Enter seat %d (e.g., A1, or 7 to exit): ", i + 1);
                            if (fgets(seatNames[i], sizeof(seatNames[i]), stdin) == NULL) {
                                printf("Error reading seat input!\n");
                                break;
                            }
                            seatNames[i][strcspn(seatNames[i], "\n")] = 0;
                            printf("Debug: Read seat %d: '%s'\n", i + 1, seatNames[i]);
                            if (strcmp(seatNames[i], "7") == 0) break;
                            if (strlen(seatNames[i]) == 2 && seatNames[i][0] >= 'A' && seatNames[i][0] <= 'E' && seatNames[i][1] >= '1' && seatNames[i][1] <= '5') {
                                valid = 1;
                            } else if (strlen(seatNames[i]) == 0) {
                                // Skip empty input and prompt again
                                continue;
                            } else {
                                printf("Invalid seat format! Enter like A1.\n");
                            }
                        }
                        if (strcmp(seatNames[i], "7") == 0) break;
                    }
                    if (strcmp(seatNames[0], "7") == 0) break;

                    if (fastCancelBooking(username, movieName, cancelDay, cancelShowtime, seatNames, numSeats)) {
                        for (int i = 0; i < numSeats; i++) {
                            int row = seatNames[i][0] - 'A';
                            int col = seatNames[i][1] - '1';
                            cancelSeat(movieName, cancelDay, cancelShowtime, row, col);
                        }
                        int prices[ROWS] = {400, 350, 300, 250, 200};
                        int totalPrice = 0;
                        for (int i = 0; i < numSeats; i++) {
                            int row = seatNames[i][0] - 'A';
                            totalPrice += prices[row];
                        }
                        printf("Booking canceled successfully! ₹%d refunded.\n", totalPrice);
                    } else {
                        printf("No such booking found!\n");
                    }
                    break;
                }

                case 5:
                    saveBookingHistoryToFile();
                    saveSeatMatrices();
                    cleanupBookingHistory();
                    printf("Exiting program...\n");
                    return 0;

                case 6:
                    resetSeatMatrices();
                    resetBookingHistory();
                    printf("System reset successfully! All seat matrices and booking history have been cleared.\n");
                    break;
            }
        } while (choice != 5);
    }

    return 0;
}