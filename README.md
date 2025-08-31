# MovieFinder — K‑NN Movie Recommender (C)

A small console application that recommends movies from the IMDb Top 250 using the **K‑Nearest Neighbors (K‑NN)** algorithm. The user enters preferred genres, a year range, a minimum IMDb rating, and optionally a favorite director or actor. The app then returns **Top‑5** personalized suggestions.  

---

## Table of Contents
- [Overview](#overview)
- [How It Works (K‑NN at a Glance)](#how-it-works-k-nn-at-a-glance)
- [Data Model](#data-model)
- [Key Functions](#key-functions)
- [Program Flow](#program-flow)
- [Usage](#usage)
- [Notes & Future Work](#notes--future-work)
- [Course Context](#course-context)
- [References](#references)

---

## Overview

**MovieFinder** is a teaching example that demonstrates how to combine file parsing, data structures, vectorization, and a classic ML method (K‑NN) to build a simple recommender in C. The implementation avoids external ML libraries and focuses on clarity and fundamentals.

---

## How It Works (K‑NN at a Glance)

**K‑NN** is a supervised, non‑parametric and “lazy” learning algorithm used for classification and regression. For a new input, it looks at the **K nearest** items in the dataset and decides based on their labels/values. In this project, we measure similarity between a **user vector** and each **movie vector** and rank by distance.

**Feature space (for both user and movie):**
- 21 binary components for genres (1 = present, 0 = absent)  
- normalized year  
- normalized IMDb rating  
- a final indicator component signaling a director/actor match  

**Distance metric:** Euclidean distance  
\[ d = \sqrt{(x_1-y_1)^2 + (x_2-y_2)^2 + \dots + (x_n-y_n)^2} \]

**Top‑K** (K nearest movies with the smallest distance) form the recommendations.

---

## Data Model

```c
typedef struct {
    char title[100];
    int year;
    float rating;
    char genre[100];
    char director[50];
    char actors[200];
} Movie;

typedef struct {
    char genres[100];     // comma-separated (e.g., "Drama, Crime")
    char years[20];       // range "YYYY-YYYY"
    float min_rating;     // e.g., 8.5
    char director[100];   // optional director or actor
} UserPref;

typedef struct {
    int index;  // index into the movies array
    float dist; // computed distance to the user vector
} MovieDistance;
```

---

## Key Functions

- `load_movies(const char* filename, Movie** movies, int* n_movies)`  
  Loads the CSV database (IMDb Top 250) into a dynamic `Movie` array (growing buffer via `realloc`), parses columns (`title, year, rating, genre, director, actors`), and strips trailing newlines.

- `strip_newline(char* str)`  
  Utility to remove the trailing `\n` from strings read with `fgets` to keep fields clean for comparison/printing.

- `user_inputs(UserPref* prefs)`  
  Collects user preferences interactively (genres, year range, minimum rating, optional director/actor).

- `movie_to_vector(const Movie* movie, float* vec, ...)`  
  Maps one movie into the shared numeric feature space.

- `user_pref_to_vector(const UserPref pref, float* vec, ...)`  
  Maps the user preferences into a vector of **the same dimension and order** as movie vectors.

- `distance(const float* v1, const float* v2, int n)`  
  Computes the Euclidean distance between a movie vector and the user vector (core of K‑NN).

- `cmp(const void* a, const void* b)`  
  `qsort` comparator for sorting `(index, distance)` pairs by ascending distance.

- `main()`  
  Orchestrates: load → input → vectorize → distance → sort → print **Top‑5**.

---

## Program Flow

1. Vectorize all movies and the user preferences.  
2. Compute Euclidean distance between the user vector and every movie vector.  
3. Sort all movies by ascending distance.  
4. Apply a coarse filter by minimum rating and (optionally) director/actor match.  
5. Output the **Top‑5** recommendations.

---

## Usage

Run the program and follow the prompts to enter preferences:

**Example input**
- Genres: `Drama, Crime`  
- Years: `1990-2010`  
- Minimum rating: `8.5`  
- Director (optional): `Christopher Nolan`

**Example of recommended titles (illustrative):**
- *The Dark Knight* (2008) — **Crime | Drama**, Christopher Nolan  
- *Pulp Fiction* (1994) — **Crime | Drama**, Quentin Tarantino  
- *The Godfather* (1972) — **Crime | Drama**, Francis Ford Coppola  

> The list is ranked by similarity to your preferences. The first items are the “nearest neighbors.”

---

## Notes & Future Work

- Expand the dataset beyond IMDb Top‑250 for broader coverage.  
- Experiment with alternative similarity metrics (e.g., Manhattan, Minkowski).  
- Add weights per feature (e.g., emphasize director/actor match).  
- Build a minimal GUI or a web front end to improve UX.

---

## Course Context

- Course: **CS130 — C/C++ Programming Language**  
- Student: **Luka Ristić**, Index **6001**  
- Project date: **2025‑06‑25**

---

## References

- GeeksforGeeks — *K‑Nearest Neighbours (K‑NN)*: https://www.geeksforgeeks.org/machine-learning/k-nearest-neighbours/  
- IBM — *What is KNN?*: https://www.ibm.com/think/topics/knn
