#include <stdio.h>
#include <assert.h>
#include "csv.h"
#include "api.h"

#include <stdlib.h>
#include <string.h>

// Get the API version information
const char *api_version() {
    return "UOC PP 20242";
}

// Load data from a CSV file. If reset is true, remove previous data
tApiError api_loadData(tApiData *data, const char *filename, bool reset) {
    tApiError error;
    FILE *fin;
    char buffer[FILE_READ_BUFFER_SIZE];
    tCSVEntry entry;

    // Check input data
    assert(data != NULL);
    assert(filename != NULL);

    // Reset current data
    if (reset) {
        // Remove previous information
        error = api_freeData(data);
        if (error != E_SUCCESS) {
            return error;
        }

        // Initialize the data
        error = api_initData(data);
        if (error != E_SUCCESS) {
            return error;
        }
    }

    // Open the input file
    fin = fopen(filename, "r");
    if (fin == NULL) {
        return E_FILE_NOT_FOUND;
    }

    // Read file line by line
    while (fgets(buffer, FILE_READ_BUFFER_SIZE, fin)) {
        // Remove new line character
        buffer[strcspn(buffer, "\n\r")] = '\0';

        csv_initEntry(&entry);
        csv_parseEntry(&entry, buffer, NULL);
        // Add this new entry to the api Data
        error = api_addDataEntry(data, entry);
        if (error != E_SUCCESS) {
            csv_freeEntry(&entry);
            fclose(fin);
            return error;
        }
        csv_freeEntry(&entry);
    }

    fclose(fin);

    return E_SUCCESS;
}

// 3b - Initialize the data structure
tApiError api_initData(tApiData *data) {
    assert(data != NULL);

    people_init(&data->people);
    subscriptions_init(&data->subscriptions);
    catalog_init(&data->catalog);

    return E_SUCCESS;
}

// 3c - Add a person into the data if it does not exist
tApiError api_addPerson(tApiData *data, tCSVEntry entry) {
    assert(data != NULL);
    tPerson newPerson;

    if (strcmp(entry.type, "PERSON") != 0) {
        return E_INVALID_ENTRY_TYPE;
    }
    if (csv_numFields(entry) != NUM_FIELDS_PERSON) {
        return E_INVALID_ENTRY_FORMAT;
    }

    person_parse(&newPerson, entry);
    if (people_find(data->people, newPerson.document) >= 0) {
        return E_PERSON_DUPLICATED;
    }

    if (data->people.count == 0) {
        // FIRST PERSON
        data->people.elems = (tPerson *) malloc(sizeof(tPerson));

        if (data->people.elems == NULL) {
            return E_MEMORY_ERROR;
        }
    } else {
        tPerson *temp = realloc(data->people.elems, (data->people.count + 1) * sizeof(tPerson));

        if (temp == NULL) {
            return E_MEMORY_ERROR;
        }

        data->people.elems = temp;
    }

    data->people.elems[data->people.count] = newPerson;
    data->people.count++;

    return E_SUCCESS;
}

// 3d - Add a subscription if it does not exist
tApiError api_addSubscription(tApiData *data, tCSVEntry entry) {
    assert(data != NULL);
    tSubscription newSubs;

    if (strcmp(entry.type, "SUBSCRIPTION") != 0) {
        return E_INVALID_ENTRY_TYPE;
    }
    if (csv_numFields(entry) != NUM_FIELDS_SUBSCRIPTION) {
        return E_INVALID_ENTRY_FORMAT;
    }

    subscription_parse(&newSubs, entry);
    if (subscriptions_find(data->subscriptions, newSubs.id) >= 0) {
        return E_SUBSCRIPTION_DUPLICATED;
    }
    if (people_find(data->people, newSubs.document) < 0) {
        return E_PERSON_NOT_FOUND;
    }

    if (data->subscriptions.count == 0) {
        // FIRST SUBSCRIPTION
        data->subscriptions.elems = (tSubscription *) malloc(sizeof(tSubscription));

        if (data->subscriptions.elems == NULL) {
            return E_MEMORY_ERROR;
        }
    } else {
        tSubscription *temp = realloc(data->subscriptions.elems,
                                      (data->subscriptions.count + 1) * sizeof(tSubscription));

        if (temp == NULL) {
            return E_MEMORY_ERROR;
        }

        data->subscriptions.elems = temp;
    }

    data->subscriptions.elems[data->subscriptions.count] = newSubs;
    data->subscriptions.count++;

    return E_SUCCESS;
}

// 3e - Add a film if it does not exist
tApiError api_addFilm(tApiData *data, tCSVEntry entry) {
    assert(data != NULL);
    tFilm newFilm;

    if (strcmp(entry.type, "FILM") != 0) {
        return E_INVALID_ENTRY_TYPE;
    }
    if (csv_numFields(entry) != NUM_FIELDS_FILM) {
        return E_INVALID_ENTRY_FORMAT;
    }

    film_parse(&newFilm, entry);

    if (filmList_find(data->catalog.filmList, newFilm.name) != NULL) {
        return E_FILM_DUPLICATED;
    }

    tFilm *filmNode = (tFilm *)malloc(sizeof(tFilm));
    if (filmNode == NULL) {
        return E_MEMORY_ERROR;
    }
    *filmNode = newFilm;

    filmList_add(&data->catalog.filmList, *filmNode);
    if (newFilm.isFree) {
        freeFilmList_add(&data->catalog.freeFilmList, filmNode);
    }

    return E_SUCCESS;
}

// Get the number of people registered on the application
int api_peopleCount(tApiData data) {
    /////////////////////////////////
    // PR1_3f
    /////////////////////////////////

    /////////////////////////////////
    return -1;
}

// Get the number of subscriptions registered on the application
int api_subscriptionsCount(tApiData data) {
    /////////////////////////////////
    // PR1_3f
    /////////////////////////////////

    /////////////////////////////////
    return -1;
}

// Get the number of films registered on the application
int api_filmsCount(tApiData data) {
    /////////////////////////////////
    // PR1_3f
    /////////////////////////////////

    /////////////////////////////////
    return -1;
}

// Get the number of free films registered on the application
int api_freeFilmsCount(tApiData data) {
    /////////////////////////////////
    // PR1_3f
    /////////////////////////////////

    /////////////////////////////////
    return -1;
}

// Free all used memory
tApiError api_freeData(tApiData *data) {
    /////////////////////////////////
    // PR1_3g
    /////////////////////////////////

    /////////////////////////////////
    return E_NOT_IMPLEMENTED;
}

// Add a new entry
tApiError api_addDataEntry(tApiData *data, tCSVEntry entry) {
    /////////////////////////////////
    // PR1_3h
    /////////////////////////////////

    /////////////////////////////////
    return E_NOT_IMPLEMENTED;
}

// Get subscription data
tApiError api_getSubscription(tApiData data, int id, tCSVEntry *entry) {
    /////////////////////////////////
    // PR1_4a
    /////////////////////////////////

    /////////////////////////////////
    return E_NOT_IMPLEMENTED;
}

// Get film data
tApiError api_getFilm(tApiData data, const char *name, tCSVEntry *entry) {
    /////////////////////////////////
    // PR1_4b
    /////////////////////////////////

    /////////////////////////////////
    return E_NOT_IMPLEMENTED;
}

// Get free films data
tApiError api_getFreeFilms(tApiData data, tCSVData *freeFilms) {
    /////////////////////////////////
    // PR1_4c
    /////////////////////////////////

    /////////////////////////////////
    return E_NOT_IMPLEMENTED;
}

// Get films data by genre
tApiError api_getFilmsByGenre(tApiData data, tCSVData *films, int genre) {
    /////////////////////////////////
    // PR1_4d
    /////////////////////////////////

    /////////////////////////////////
    return E_NOT_IMPLEMENTED;
}
