#define _CRT_SECURE_NO_WARNINGS
#define MAX_LENGTH 512
#define N_GENRES 21
#define MIN_YEAR 1920
#define MAX_YEAR 2025

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//STRUCTURES

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

typedef struct {
    int index;
    float dist;
}MovieDistance;

//FUNCTIONS

void strip_newline(char* str);

int load_movies(const char* filename, Movie** movies, int* n_movies);

void user_inputs(UserPref* prefs);

void movie_to_vector(const Movie* movie, float* vec, int vec_size,
    const char** genres_list, int n_genres, int min_year, int max_year,
    const char* director_pref);

void user_pref_to_vector(const UserPref pref, float* vec, int vec_size,
    const char** genres_list, int n_genres, int min_year, int max_year,
    const char* director_pref);

float distance(const float* v1, const float* v2, int n);

int cmp(const void* a, const void* b);

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

    float user_vector[N_GENRES + 3] = { 0 };
    user_pref_to_vector(userPrefs, user_vector, N_GENRES + 3, genres_list, N_GENRES, MIN_YEAR, MAX_YEAR, userPrefs.director);

    float movie_vector[N_GENRES + 3];
    MovieDistance* m_distance = malloc(n_movies * sizeof(MovieDistance));

    int count = 0;
    for (int i = 0; i < n_movies; i++) {
        if (movies[i].rating < userPrefs.min_rating) continue;
        movie_to_vector(&movies[i], movie_vector, N_GENRES + 3, genres_list, N_GENRES, MIN_YEAR, MAX_YEAR, userPrefs.director);
        m_distance[count].index = i;
        m_distance[count].dist = distance(user_vector, movie_vector, N_GENRES + 3);
        count++;
    }

    if (count == 0) {
        printf("No movies found matching your preferences!\n");
        free(movies);
        free(m_distance);
        return 1;
    }

    
    qsort(m_distance, count, sizeof(MovieDistance), cmp);

    for (int i = 0; i < 5 && i < count; i++) {
        int index = m_distance[i].index;
        printf("%d. %s (%d) - Rating %.1f, Genres: %s, Director: %s\n", i + 1, movies[index].title, movies[index].year, movies[index].rating, movies[index].genre, movies[index].director);
    }

    
    free(movies);
    free(m_distance);
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
        if (token) {
            strcpy(m.title, token);
            strip_newline(m.title);
        }

        token = strtok(NULL, ",");
        if (token) m.year = atoi(token);

        token = strtok(NULL, ",");
        if (token) m.rating = atof(token);

        token = strtok(NULL, ",");
        if (token) {
            strcpy(m.genre, token);
            strip_newline(m.genre);
        }

        token = strtok(NULL, ",");
        if (token) {
            strcpy(m.director, token);
            strip_newline(m.director);
        }

        token = strtok(NULL, "\n");
        if (token) {
            strcpy(m.actors, token);
            strip_newline(m.actors);
        }

        (*movies)[*n_movies] = m;
        (*n_movies)++;

    }

    fclose(data);
    return 1;
}

void strip_newline(char* str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

void user_inputs(UserPref *prefs) {
    printf("Enter your favorite genres (separate multiple genres with a comma):\n> ");
    fgets(prefs->genres, 100, stdin);
    strip_newline(prefs->genres);

    printf("\nEnter the desired year range (e.g., 1990-2010):\n> ");
    fgets(prefs->years, 20, stdin);
    strip_newline(prefs->years);

    printf("\nEnter the minimum IMDb rating (e.g., 8.0):\n> ");
    scanf("%f", &prefs->min_rating);
    getchar(); 

    printf("\n(Optional) Enter your favorite director or actor (or leave blank):\n> ");
    fgets(prefs->director, 100, stdin);
    strip_newline(prefs->director);
}

void movie_to_vector(const Movie* movie, float* vec, int vec_size,
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

float distance(const float* v1, const float* v2, int n) {
    float sum = 0.0;

    for (int i = 0; i < n; i++) {
        float diff = v1[i] - v2[i];
        if (i == n - 1)
            diff *= 3;
        sum += pow(diff, 2);
    }

    return sqrt(sum);
}

int cmp(const void* a, const void* b) {
    float d = ((MovieDistance*)a)->dist - ((MovieDistance*)b)->dist;
    return (d > 0) - (d < 0);
}