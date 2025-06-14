#define _CRT_SECURE_NO_WARNINGS
#define MAX_LENGTH 512
#define N_GENRES 21

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char title[100];
    int year;
    float rating;
    char genre[100];
    char director[50];
    char actors[200];
}Movie;

typedef struct {
    char genres[100];
    char years[20];
    float min_rating;
    char director[100];
}UserPref;

int load_movies(const char* filename, Movie** movies, int* n_movies);

void user_inputs(UserPref* prefs);

void film_to_vector(const Movie* movie, float* vec, int vec_size,
    const char** genres_list, int n_genres, int min_year, int max_year,
    const char* director_pref);

void user_pref_to_vector(const UserPref pref, float* vec, int vec_size,
    const char** genres_list, int n_genres, int min_year, int max_year,
    const char* director_pref);

int main() {
    Movie *movies = NULL;
    UserPref userPrefs;
    int n_movies = 0;
    const char* genres_list[N_GENRES] = {
    "Drama",
    "Adventure",
    "Thriller",
    "Crime",
    "Comedy",
    "Action",
    "Mystery",
    "War",
    "Fantasy",
    "Sci-Fi",
    "Biography",
    "Family",
    "Romance",
    "Animation",
    "History",
    "Horror",
    "Sport",
    "Music",
    "Western",
    "Musical",
    "Film-Noir"
    };
    
    


    if (!load_movies("movies.csv", &movies, &n_movies)) {
        printf("Error while fetching movies!\n");
        return 0;
    }

    printf("============================================\n");
    printf("          Welcome to MovieFinder!        \n");
    printf("============================================\n\n");
    printf("Find your perfect movie with the power of AI recommendations!\n");
    printf("Let's set your preferences to get started.\n\n");
    
    user_inputs(&userPrefs);

    //za testiranje only - obrisati kasnije
    float user_vec[N_GENRES + 3] = { 0 }; // žanrovi + godina + ocena + reditelj
    user_pref_to_vector(userPrefs, user_vec, N_GENRES + 3, userPrefs.genres, N_GENRES, userPrefs.years, 2025, userPrefs.director);

    printf("User vector:\n");
    for (int i = 0; i < N_GENRES + 3; i++) {
        printf("%.2f ", user_vec[i]);
    }
    printf("\n");

    free(movies);
    return 1;
}

int load_movies(const char* filename, Movie** movies, int* n_movies) {
    FILE* data = fopen(filename, "r");
    if (!data) {
        printf("An error has occured! Data from databaes fetch failed.\n");
        return 0;
    }

    char line[MAX_LENGTH];
    int size = 100;
    *n_movies = 0;
    *movies = malloc(size * sizeof(Movie));

    if (!movies) {
        printf("An error has occured! Not enough memory :(\n");
        fclose(data);
        return 0;
    }

    fgets(line, MAX_LENGTH, data); //skippuje prvu liniju

    while (fgets(line, MAX_LENGTH, data) != NULL) {
        if (*n_movies >= size) {
            size *= 2;
            *movies = realloc(*movies, size * sizeof(Movie));
            if (!*movies) {
                printf("An error has occured! Not enough memory :(\n");
                fclose(data);
                return 0;
            }
        }

        Movie m;
        char* token = strtok(line, ",");
        if (token) strcpy(m.title, token);

        token = strtok(NULL, ",");
        if (token) m.year = atoi(token);

        token = strtok(NULL, ",");
        if (token) m.rating = atof(token);

        token = strtok(NULL, ",");
        if (token) strcpy(m.genre, token);

        token = strtok(NULL, ",");
        if (token) strcpy(m.director, token);

        token = strtok(NULL, "\n");
        if (token) strcpy(m.actors, token);

        (*movies)[*n_movies] = m;
        (*n_movies)++;

    }

    fclose(data);
    return 1;
}

void user_inputs(UserPref *prefs) {
    printf("Enter your favorite genres (separate multiple genres with a comma):\n> ");
    fgets(prefs->genres, 100, stdin);

    printf("\nEnter the desired year range (e.g., 1990-2010):\n> ");
    fgets(prefs->years, 20, stdin);

    printf("\nEnter the minimum IMDb rating (e.g., 8.0):\n> ");
    scanf("%f", &prefs->min_rating);
    getchar(); 

    printf("\n(Optional) Enter your favorite director or actor (or leave blank):\n> ");
    fgets(prefs->director, 100, stdin);
}

void film_to_vector(const Movie* movie, float* vec, int vec_size,
    const char** genres_list, int n_genres, int min_year, int max_year,
    const char* director_pref) {

    //bin deo za zarnove
    for (int i = 0; i < n_genres; i++) {
        if (strstr(movie->genre, genres_list[i]) != NULL)
            vec[i] = 1.0;
        else
            vec[i] = 0.0;
    }

    vec[n_genres] = (movie->year - min_year) / (float)(max_year - min_year); // normalizacija godine
    vec[n_genres + 1] = (movie->rating - 1.0) / 9.0; // normalizacija ocene

    if (director_pref && strlen(director_pref) > 1 && strstr(movie->director, director_pref) != NULL)
        vec[n_genres + 2] = 1.0;
    else
        vec[n_genres + 2] = 0.0;
}

void user_pref_to_vector(const UserPref pref, float* vec, int vec_size,
    const char** genres_list, int n_genres, int min_year, int max_year,
    const char* director_pref) {

    //bin deo za zarnove
    for (int i = 0; i < n_genres; i++) {
        if (strstr(pref.genres, genres_list[i]) != NULL)
            vec[i] = 1.0;
        else
            vec[i] = 0.0;
    }

    int start_year = min_year;
    int end_year = min_year;
    sscanf(pref.years, "%d-%d", &start_year, &end_year);
    int user_year = (start_year + end_year) / 2;
    vec[n_genres] = (user_year - min_year) / (float)(max_year - min_year); // normalizacija godine

    vec[n_genres + 1] = (pref.min_rating - 1.0) / 9.0; // normalizacija ocene

    if (director_pref && strlen(director_pref) > 1 && strstr(pref.director, director_pref) != NULL)
        vec[n_genres + 2] = 1.0;
    else
        vec[n_genres + 2] = 0.0;
}