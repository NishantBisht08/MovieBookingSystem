#include "movie_list.h"

Movie *movieTable[TABLE_SIZE];

unsigned int hash(char *name) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash << 5) + *name++;
    }
    return hash % TABLE_SIZE;
}

void initMovieTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        movieTable[i] = NULL;
    }

    int showtimes[NUM_SHOWTIMES] = {1430, 1700, 1930, 2100, 2300};
    int seatMatrix[NUM_SHOWTIMES][ROWS][COLS] = {0};

    insertMovie("The Matrix", "Sci-Fi", 1, showtimes, seatMatrix);
    insertMovie("Dune", "Sci-Fi", 2, showtimes, seatMatrix);
    insertMovie("Inception", "Thriller", 3, showtimes, seatMatrix);
    insertMovie("Avatar", "Action", 4, showtimes, seatMatrix);
    insertMovie("Titanic", "Romance", 5, showtimes, seatMatrix);

    loadSeatMatrices();
}

void insertMovie(char *name, char *genre, int day, int showtimes[], int seatMatrix[][ROWS][COLS]) {
    Movie *movie = (Movie *)malloc(sizeof(Movie));
    strcpy(movie->name, name);
    strcpy(movie->genre, genre);
    movie->day = day;
    for (int i = 0; i < NUM_SHOWTIMES; i++) {
        movie->showtimes[i] = showtimes[i];
        for (int j = 0; j < ROWS; j++) {
            for (int k = 0; k < COLS; k++) {
                movie->seatMatrix[i][j][k] = seatMatrix[i][j][k];
            }
        }
    }
    movie->next = NULL;

    unsigned int index = hash(name);
    if (movieTable[index] == NULL) {
        movieTable[index] = movie;
    } else {
        Movie *current = movieTable[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = movie;
    }
}

Movie* searchMovie(char *name) {
    unsigned int index = hash(name);
    Movie *current = movieTable[index];
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void displayMoviesForDay(int day) {
    int found = 0;
    printf("\nMovies for Day %d:\n", day);
    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie *current = movieTable[i];
        while (current != NULL) {
            if (current->day == day) {
                found++;
                printf("%d. %s (%s)\n", found, current->name, current->genre);
            }
            current = current->next;
        }
    }
    if (!found) {
        printf("No movies available on Day %d.\n", day);
    }
}

void displayShowtimes(char *movieName, int day) {
    Movie *movie = searchMovie(movieName);
    if (movie && movie->day == day) {
        printf("\nShowtimes for %s on Day %d:\n", movieName, day);
        for (int i = 0; i < NUM_SHOWTIMES; i++) {
            printf("%d. %02d:%02d\n", i + 1, movie->showtimes[i] / 100, movie->showtimes[i] % 100);
        }
    } else {
        printf("Movie not found or not scheduled on Day %d.\n", day);
    }
}

void saveSeatMatrices() {
    FILE *file = fopen("seat_reservations.txt", "w");
    if (!file) {
        printf("Error opening seat_reservations.txt for writing.\n");
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie *current = movieTable[i];
        while (current != NULL) {
            fprintf(file, "%s,%d\n", current->name, current->day);
            for (int s = 0; s < NUM_SHOWTIMES; s++) {
                fprintf(file, "%02d:%02d\n", current->showtimes[s] / 100, current->showtimes[s] % 100);
                for (int r = 0; r < ROWS; r++) {
                    for (int c = 0; c < COLS; c++) {
                        fprintf(file, "%d ", current->seatMatrix[s][r][c]);
                    }
                    fprintf(file, "\n");
                }
            }
            current = current->next;
        }
    }

    fclose(file);
}

void loadSeatMatrices() {
    FILE *file = fopen("seat_reservations.txt", "r");
    if (!file) {
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char movieName[50];
        int day;
        sscanf(line, "%[^,],%d", movieName, &day);
        Movie *movie = searchMovie(movieName);
        if (movie && movie->day == day) {
            for (int s = 0; s < NUM_SHOWTIMES; s++) {
                fgets(line, sizeof(line), file);
                for (int r = 0; r < ROWS; r++) {
                    fgets(line, sizeof(line), file);
                    int c = 0;
                    char *token = strtok(line, " \n");
                    while (token && c < COLS) {
                        movie->seatMatrix[s][r][c] = atoi(token);
                        token = strtok(NULL, " \n");
                        c++;
                    }
                }
            }
        } else {
            for (int s = 0; s < NUM_SHOWTIMES; s++) {
                fgets(line, sizeof(line), file);
                for (int r = 0; r < ROWS; r++) {
                    fgets(line, sizeof(line), file);
                }
            }
        }
    }

    fclose(file);
}

void resetSeatMatrices() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Movie *current = movieTable[i];
        while (current != NULL) {
            for (int s = 0; s < NUM_SHOWTIMES; s++) {
                for (int r = 0; r < ROWS; r++) {
                    for (int c = 0; c < COLS; c++) {
                        current->seatMatrix[s][r][c] = 0; // Set all seats to available
                    }
                }
            }
            current = current->next;
        }
    }
    saveSeatMatrices(); // Save the reset state to seat_reservations.txt
}