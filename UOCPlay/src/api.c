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

    tFilm *filmNode = (tFilm *) malloc(sizeof(tFilm));
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

// 3f.1 - Get the number of people registered on the application
int api_peopleCount(tApiData data) {
    return data.people.count;
}

// 3f.2 - Get the number of subscriptions registered on the application
int api_subscriptionsCount(tApiData data) {
    return data.subscriptions.count;
}

// 3f.3 - Get the number of films registered on the application
int api_filmsCount(tApiData data) {
    return data.catalog.filmList.count;
}

// 3f.4 - Get the number of free films registered on the application
int api_freeFilmsCount(tApiData data) {
    return data.catalog.freeFilmList.count;
}

// 3g - Free all used memory
tApiError api_freeData(tApiData *data) {
    assert(data != NULL);
    if (data == NULL) {
        return E_SUCCESS;
    }

    people_free(&data->people);
    catalog_free(&data->catalog);
    subscriptions_free(&data->subscriptions);

    return E_SUCCESS;
}

// 3h - Add a new entry
tApiError api_addDataEntry(tApiData *data, tCSVEntry entry) {
    assert(data != NULL);
    assert(
        (strcmp(entry.type, "PERSON") == 0) ||
        (strcmp(entry.type, "SUBSCRIPTION") == 0) ||
        (strcmp(entry.type, "FILM") == 0));
    // PARSE + ADD
    if (strcmp(entry.type, "PERSON") == 0) {
        tPerson newPerson;

        person_parse(&newPerson, entry);
        people_add(&data->people, newPerson);
    }
    if (strcmp(entry.type, "FILM") == 0) {
        tFilm newFilm;

        film_parse(&newFilm, entry);
        catalog_add(&data->catalog, newFilm);
    }
    if (strcmp(entry.type, "SUBSCRIPTION") == 0) {
        tSubscription newSubs;

        subscription_parse(&newSubs, entry);
        subscriptions_add(&data->subscriptions, data->people, newSubs);
    }

    return E_SUCCESS;
}

// 4a - Get subscription data
tApiError api_getSubscription(tApiData data, const int id, tCSVEntry *entry) {
    assert(data.subscriptions.elems != NULL);
    assert(entry != NULL);
    csv_initEntry(entry); // EMPTY ENTRY
    char buffer[FILE_READ_BUFFER_SIZE];

    const int found = subscriptions_find(data.subscriptions, id);
    if (found < 0) {
        return E_SUBSCRIPTION_NOT_FOUND;
    };
    tSubscription subsFound = data.subscriptions.elems[found];

    // FORMAT ENTRY
    entry->type = (char *) malloc(strlen("SUBSCRIPTION") + 1);
    strcpy(entry->type, "SUBSCRIPTION");
    entry->numFields = NUM_FIELDS_SUBSCRIPTION;
    entry->fields = (char **) malloc(sizeof(char *) * entry->numFields);
    // ID
    snprintf(buffer, sizeof(buffer), "%d", subsFound.id);
    entry->fields[0] = strdup(buffer);
    // DOCUMENT
    entry->fields[1] = strdup(subsFound.document);
    // START DATE
    date_format(subsFound.start_date, buffer);
    entry->fields[2] = strdup(buffer);
    // END DATE
    date_format(subsFound.end_date, buffer);
    entry->fields[3] = strdup(buffer);
    // PLAN
    entry->fields[4] = strdup(subsFound.plan);
    // PRICE
    if (subsFound.price == (int) subsFound.price) {
        snprintf(buffer, sizeof(buffer), "%d", (int)subsFound.price);
    } else {
        snprintf(buffer, sizeof(buffer), "%.2f", subsFound.price);
    }
    entry->fields[5] = strdup(buffer);
    // NUM DEVICES
    snprintf(buffer, sizeof(buffer), "%d", subsFound.numDevices);
    entry->fields[6] = strdup(buffer);

    return E_SUCCESS;
}

// 4b - Get film data
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

void date_format(const tDate date, char* buffer) {
    // dd/mm/yyyy -> BUFFER
    snprintf(buffer, 11, "%02d/%02d/%04d", date.day, date.month, date.year);
}