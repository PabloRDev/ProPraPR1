#include "film.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// Parse input from CSVEntry
void film_parse(tFilm *data, tCSVEntry entry) {
    // Check input data
    assert(data != NULL);
    assert(csv_numFields(entry) == NUM_FIELDS_FILM);

    int pos = 0;

    // Name
    const char *name = entry.fields[pos++];
    assert(name != NULL);

    // Duration
    assert(strlen(entry.fields[pos]) == TIME_LENGTH);
    tTime duration;
    int itemsRead = sscanf(entry.fields[pos++], "%d:%d", &duration.hour, &duration.minutes);
    assert(itemsRead == 2);

    // Genre
    int genreValue = csv_getAsInteger(entry, pos++);
    assert(genreValue >= GENRE_FIRST && genreValue < GENRE_END);
    tFilmGenre genre = (tFilmGenre) genreValue;

    // Release date
    assert(strlen(entry.fields[pos]) == DATE_LENGTH);
    tDate release;
    itemsRead = sscanf(entry.fields[pos++], "%d/%d/%d", &release.day, &release.month, &release.year);
    assert(itemsRead == 3);

    // Rating
    float rating = csv_getAsReal(entry, pos++);
    assert(rating >= RATING_MIN && rating <= RATING_MAX);

    // isFree
    int isFree = csv_getAsInteger(entry, pos++);
    assert(isFree == 0 || isFree == 1);

    // Call film_init with the parsed data
    film_init(data, name, duration, genre, release, rating, (bool) isFree);
}

// Initialize a film
void film_init(tFilm *data, const char *name, tTime duration, tFilmGenre genre, tDate release, float rating,
               bool isFree) {
    // Check preconditions
    assert(data != NULL);
    assert(name != NULL);

    // Name
    data->name = (char *) malloc((strlen(name) + 1) * sizeof(char));
    assert(data->name != NULL);
    strcpy(data->name, name);

    // Duration
    time_cpy(&data->duration, duration);

    // Genre
    data->genre = genre;

    // Release
    date_cpy(&data->release, release);

    // Rating
    data->rating = rating;

    // isFree
    data->isFree = isFree;
}

// Copy a film from src to dst
void film_cpy(tFilm *dst, tFilm src) {
    // Check preconditions
    assert(dst != NULL);

    film_init(dst, src.name, src.duration, src.genre, src.release, src.rating, src.isFree);
}

// Get film data using a string
void film_get(tFilm data, char *buffer) {
    // Print all data at same time
    sprintf(buffer, "%s;%02d:%02d;%d;%02d/%02d/%04d;%.1f;%d",
            data.name,
            data.duration.hour, data.duration.minutes,
            data.genre,
            data.release.day, data.release.month, data.release.year,
            data.rating,
            data.isFree);
}

// Remove the data from a film
void film_free(tFilm *data) {
    // Check preconditions
    assert(data != NULL);

    if (data->name != NULL) {
        free(data->name);
        data->name = NULL;
    }
}

// Initialize the films list
tApiError filmList_init(tFilmList *list) {
    // Check preconditions
    assert(list != NULL);

    list->first = NULL;
    list->last = NULL;
    list->count = 0;

    return E_SUCCESS;
}

// Add a new film to the list
tApiError filmList_add(tFilmList *list, tFilm film) {
    // Check preconditions
    assert(list != NULL);

    tFilmListNode *node;

    // Check if the film is already in the list
    if (filmList_find(*list, film.name) != NULL)
        return E_FILM_DUPLICATED;

    // Create the node
    node = (tFilmListNode *) malloc(sizeof(tFilmListNode));
    assert(node != NULL);

    // Assign the properties of the nodes
    film_cpy(&node->elem, film);
    node->next = NULL;

    // Link the new node to the end of the list
    if (list->first == NULL)
        list->first = node;
    else
        list->last->next = node;

    list->last = node;
    list->count++;

    return E_SUCCESS;
}

// Remove a film from the list
tApiError filmList_del(tFilmList *list, const char *name) {
    // Check preconditions
    assert(list != NULL);
    assert(name != NULL);

    tFilmListNode *node, *prev;

    // Iterate until the node and remove it
    node = list->first;
    prev = NULL;

    while (node != NULL) {
        if (strcmp(node->elem.name, name) == 0)
            break;

        prev = node;
        node = node->next;
    }

    // If node does not exist, return an error
    if (node == NULL)
        return E_FILM_NOT_FOUND;

    // Link the list without the node to remove
    if (prev == NULL)
        list->first = node->next;
    else
        prev->next = node->next;

    if (list->last == node)
        list->last = prev;

    list->count--;

    film_free(&(node->elem));
    free(node);

    return E_SUCCESS;
}

// Return a pointer to the film
tFilm *filmList_find(tFilmList list, const char *name) {
    // Check preconditions
    assert(name != NULL);

    tFilmListNode *node;
    node = list.first;

    while (node != NULL) {
        if (strcmp(node->elem.name, name) == 0)
            return &(node->elem);

        node = node->next;
    }

    return NULL;
}

// Remove the films from the list
tApiError filmList_free(tFilmList *list) {
    // Check preconditions
    assert(list != NULL);

    tFilmListNode *node, *auxNode;

    node = list->first;
    auxNode = NULL;

    while (node != NULL) {
        auxNode = node->next;

        film_free(&(node->elem));
        free(node);

        node = auxNode;
    }

    filmList_init(list);

    return E_SUCCESS;
}

// Initialize the free films list
tApiError freeFilmList_init(tFreeFilmList *list) {
    // Check preconditions
    assert(list != NULL);

    list->first = NULL;
    list->last = NULL;
    list->count = 0;

    return E_SUCCESS;
}

// Add a new free film to the list
tApiError freeFilmList_add(tFreeFilmList *list, tFilm *film) {
    // Check preconditions
    assert(list != NULL);
    assert(film != NULL);

    if (freeFilmList_find(*list, film->name) != NULL)
        return E_FILM_DUPLICATED;

    tFreeFilmListNode *node = (tFreeFilmListNode *) malloc(sizeof(tFreeFilmListNode));
    assert(node != NULL);

    node->elem = film; // Store the reference
    node->next = NULL;

    if (list->first == NULL)
        list->first = node;
    else
        list->last->next = node;

    list->last = node;
    list->count++;

    return E_SUCCESS;
}

// Remove a free film from the list
tApiError freeFilmList_del(tFreeFilmList *list, const char *name) {
    // Check preconditions
    assert(list != NULL);
    assert(name != NULL);

    tFreeFilmListNode *node = list->first, *prev = NULL;

    while (node != NULL) {
        if (strcmp(node->elem->name, name) == 0)
            break;
        prev = node;
        node = node->next;
    }

    if (node == NULL)
        return E_FILM_NOT_FOUND;

    if (prev == NULL)
        list->first = node->next;
    else
        prev->next = node->next;

    if (list->last == node)
        list->last = prev;

    free(node);
    list->count--;

    return E_SUCCESS;
}

// Return a pointer to the free film
tFilm *freeFilmList_find(tFreeFilmList list, const char *name) {
    // Check preconditions
    assert(name != NULL);

    tFreeFilmListNode *node;
    node = list.first;

    while (node != NULL) {
        if (strcmp(node->elem->name, name) == 0)
            return node->elem;

        node = node->next;
    }

    return NULL;
}

// Remove the free films from the list
tApiError freeFilmsList_free(tFreeFilmList *list) {
    // Check preconditions
    assert(list != NULL);

    tFreeFilmListNode *node, *auxNode;

    node = list->first;
    auxNode = NULL;

    while (node != NULL) {
        auxNode = node->next;
        free(node);
        node = auxNode;
    }

    freeFilmList_init(list);

    return E_SUCCESS;
}

// 2a - Initialize the films catalog
tApiError catalog_init(tCatalog *catalog) {
    catalog->filmList.count = 0;
    catalog->filmList.first = NULL;
    catalog->filmList.last = NULL;

    catalog->freeFilmList.count = 0;
    catalog->freeFilmList.first = NULL;
    catalog->freeFilmList.last = NULL;

    return E_SUCCESS;
}

// 2b - Add a new film to the catalog
tApiError catalog_add(tCatalog *catalog, tFilm film) {
    const bool existingFilmList = catalog->filmList.count > 0;
    const tFilm *existingFilm = filmList_find(catalog->filmList, film.name);
    if (existingFilmList && (existingFilm != NULL)) {
        // FOUND IN CATALOG
        return E_FILM_DUPLICATED;
    }
    // ALLOCATE NEW NODE
    tFilmListNode *newNode = malloc(sizeof(tFilmListNode));
    if (newNode == NULL) {
        return E_MEMORY_ERROR;
    }
    newNode->elem.name = malloc(strlen(film.name) + 1);
    if (newNode->elem.name == NULL) {
        free(newNode);
        return E_MEMORY_ERROR;
    }
    // NEW NODE = FILM
    strcpy(newNode->elem.name, film.name);
    newNode->elem.duration = film.duration;
    newNode->elem.genre = film.genre;
    newNode->elem.release = film.release;
    newNode->elem.rating = film.rating;
    newNode->elem.isFree = film.isFree;
    newNode->next = NULL; // ... -> [NEW NODE] -> NULL

    if (catalog->filmList.first == NULL) {
        // FILM LIST EMPTY
        catalog->filmList.first = newNode;
        catalog->filmList.last = newNode;
    } else {
        // ... -> LAST -> [NEW NODE] -> NULL
        catalog->filmList.last->next = newNode;
        catalog->filmList.last = newNode;
    }
    catalog->filmList.count++;

    if (film.isFree) {
        // FREE FILM
        tFreeFilmListNode *newFreeNode = malloc(sizeof(tFreeFilmListNode));
        if (newFreeNode == NULL) {
            free(newNode->elem.name);
            free(newNode);

            return E_MEMORY_ERROR;
        }
        newFreeNode->elem = &newNode->elem; // ONLY FILM POINTER
        newFreeNode->next = NULL;

        if (catalog->freeFilmList.first == NULL) {
            catalog->freeFilmList.first = newFreeNode;
            catalog->freeFilmList.last = newFreeNode;
        } else {
            catalog->freeFilmList.last->next = newFreeNode;
            catalog->freeFilmList.last = newFreeNode;
        }
        catalog->freeFilmList.count++;
    }

    return E_SUCCESS;
}

// 2c - Remove a film from the catalog
tApiError catalog_del(tCatalog *catalog, const char *name) {
    assert(catalog != NULL);
    assert(name != NULL);

    tFilmListNode *tempFirst = catalog->filmList.first;
    tFilmListNode *tempLast = catalog->filmList.last;
    tFilmListNode *prev = NULL;
    tFilmListNode *current = tempFirst;
    const tFilm *existingFilm = filmList_find(catalog->filmList, name);

    if (existingFilm == NULL) {
        // FILM NOT FOUND
        return E_FILM_NOT_FOUND;
    }
    if (tempFirst == NULL) {
        // EMPTY FILM LIST
        catalog->filmList.last = NULL;

        return E_SUCCESS;
    }
    // FREE FILM
    if (existingFilm->isFree) {
        freeFilmList_del(&catalog->freeFilmList, name);
    }
    //ONE NODE
    if (tempFirst == tempLast) {
        catalog->filmList.first = NULL;
        catalog->filmList.last = NULL;
        catalog->filmList.count = 0;

        free(tempFirst->elem.name);
        free(tempFirst);

        return E_SUCCESS;
    }
    // FIRST NODE
    if (strcmp(existingFilm->name, tempFirst->elem.name) == 0) {
        // (FIRST) [FIRST.NEXT] -> ...
        catalog->filmList.first = catalog->filmList.first->next;

        free(tempFirst->elem.name);
        free(tempFirst);
        catalog->filmList.count--;

        return E_SUCCESS;
    }
    // LAST OR OTHER
    while (current != NULL && strcmp(current->elem.name, name) != 0) {
        // FIRST -> ... (-> PREV -> CURRENT) -> [CURRENT.NEXT] -> ... -> LAST
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return E_FILM_NOT_FOUND;
    }

    if (current == tempLast) {
        catalog->filmList.last = prev;
        prev->next = NULL;
    } else {
        prev->next = current->next;
    }

    free(current->elem.name);
    free(current);
    catalog->filmList.count--;

    return E_SUCCESS;
}

// 2d.1 - Return the number of total films
int catalog_len(tCatalog catalog) {
    /////////////////////////////////
    // PR1_2d
    /////////////////////////////////

    /////////////////////////////////
    return -1;
}

// 2d.2 Return the number of free films
int catalog_freeLen(tCatalog catalog) {
    /////////////////////////////////
    // PR1_2d
    /////////////////////////////////

    /////////////////////////////////
    return -1;
}

// Remove the films from the catalog
tApiError catalog_free(tCatalog *catalog) {
    /////////////////////////////////
    // PR1_2e
    /////////////////////////////////

    /////////////////////////////////
    return E_NOT_IMPLEMENTED;
}
