/***********
 ID:211560065
 NAME: Halel Marmor
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_HOUR 99
#define MAX_MINUTE 59
#define MAX_SECOND 59
#define EPISODE_LENGTH 8
#define HOUR_INDEX 0
#define COLON1_INDEX 2
#define MINUTE_INDEX 3
#define COLON2_INDEX 5
#define SECOND_INDEX 6
#define DIGIT_PAIR_SIZE 3

typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

TVShow ***database = NULL;
int dbSize = 0;

char *getString();
int getInt();

int validLength(char *s);
int countShows();

void shrinkDB();
void expandDB();

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

TVShow *findShow(char *name);
Season *findSeason(TVShow *show, char *name);
Episode *findEpisode(Season *season, char *name);

void addShow();
void addSeason();
void addEpisode();

void deleteShow();
void deleteSeason();
void deleteEpisode();

void printEpisode();
void printShow();
void printArray();

void addMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}
// helper function to read a dynamic string from input. Reallocates memory 1 byte at a time (char by char).
char *getString() {
    int capacity = 1;
    int length = 0;
    char *str = malloc(capacity * sizeof(char));
    if (str == NULL) {
        return NULL;
    }
    char c;
    while ((c = getchar()) != '\n') {
        if (length + 1 >= capacity) {
            capacity++;
            char *temp = realloc(str, capacity * sizeof(char));
            if (temp == NULL) {
                free(str);
                return NULL;
            }
            str = temp;
        }
        str[length] = c;
        length++;
    }
    str[length] = '\0';
    return str;
}
int getInt(){
    int num;
    scanf("%d",&num);
    getchar();
    return num;
}

int validLength(char *s) {
    int length = strlen(s);
    if (length != EPISODE_LENGTH) {
        return 0;
    }
    for (int i = 0; i < EPISODE_LENGTH ; i++) {
        if (i == COLON1_INDEX || i == COLON2_INDEX) {
            if (s[i] != ':'){
            return 0;
            }
        } else {
            if (s[i] < '0' || s[i] > '9') {
                return 0;
            }
        }
    }
    char temp[DIGIT_PAIR_SIZE];
    temp[0] = s[HOUR_INDEX];
    temp[1] = s[HOUR_INDEX + 1];
    temp[2] = '\0';
    int hours = atoi(temp);
    temp[0] = s[MINUTE_INDEX];
    temp[1] = s[MINUTE_INDEX + 1];
    temp[2] = '\0';
    int minutes = atoi(temp);
    temp[0] = s[SECOND_INDEX];
    temp[1] = s[SECOND_INDEX + 1];
    temp[2] = '\0';
    int seconds = atoi(temp);

    if (hours > MAX_HOUR || minutes > MAX_MINUTE || seconds > MAX_SECOND) {
        return 0;
    }
    return 1;
}

int countShows() {
    int count = 0;
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) {
                count++;
            }
        }
    }
    return count;
}

void expandDB(){
    int newSize = dbSize + 1;
    int numShows = countShows();
//allocate new rows
    TVShow ***newDB = malloc(sizeof(TVShow**) * newSize);
//allocate columns for each row
    for (int i = 0; i < newSize; i++) {
        newDB[i] = malloc(newSize * sizeof(TVShow*));
    }
//initialize everything to NULL
    for (int i = 0; i < newSize; i++) {
        for (int j = 0; j < newSize; j++) {
            newDB[i][j] = NULL;
        }
    }
//copy existing shows to the new array linearly
    int idx = 0;
    for (int i = 0; i < dbSize && idx < numShows; i++) {
        for (int j = 0; j < dbSize && idx < numShows; j++) {
            if (database[i][j] != NULL) {
                int newRow = idx / newSize;
                int newCol = idx % newSize;
                newDB[newRow][newCol] = database[i][j];
                idx++;
            }
        }
    }
//free old database structure
    for (int i = 0; i < dbSize; i++) {
        free(database[i]);
    }
    free(database);
    database = newDB;
    dbSize = newSize;
}

void shrinkDB() {
    if (dbSize == 0) {
        return;
    }
    int newSize = dbSize - 1;
    if (newSize == 0) {
        for (int i = 0; i < dbSize; i++) {
            free(database[i]);
        }
        free(database);
        database = NULL;
        dbSize = 0;
        return;
    }
    int numShows = countShows();
    TVShow ***newDB = malloc(sizeof(TVShow**) * newSize);
//allocate new smaller array
    for (int i = 0; i < newSize; i++) {
        newDB[i] = malloc(newSize * sizeof(TVShow*));
    }
//initialize to NULL
    for (int i = 0; i < newSize; i++) {
        for (int j = 0; j < newSize; j++) {
            newDB[i][j] = NULL;
        }
    }
//copy shows
    int idx = 0;
    for (int i = 0; i < dbSize && idx < numShows; i++) {
        for (int j = 0; j < dbSize && idx < numShows; j++) {
            if (database[i][j] != NULL) {
                int newRow = idx / newSize;
                int newCol = idx % newSize;
                newDB[newRow][newCol] = database[i][j];
                idx++;
            }
        }
    }
//free old database
    for (int i = 0; i < dbSize; i++) {
        free(database[i]);
    }
    free(database);
    database = newDB;
    dbSize = newSize;
}

void freeEpisode(Episode *e) {
    if (e == NULL) {
        return;
    }
    free(e->name);
    free(e->length);
    free(e);
}
void freeSeason(Season *s) {
    if (s == NULL) {
        return;
    }
    Episode *ep = s->episodes;
    while (ep != NULL) {
        Episode *temp = ep;
        ep = ep->next;
        freeEpisode(temp);
    }
    free(s->name);
    free(s);
}
void freeShow(TVShow *show){
    if (show == NULL) {
        return;
    }
    Season *season = show->seasons;
    while (season != NULL) {
        Season *temp = season;
        season = season->next;
        freeSeason(temp);
    }
    free(show->name);
    free(show);
}

void freeAll() {
    if (database == NULL) {
        return;
    }
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) {
                freeShow(database[i][j]);
            }
        }
        free(database[i]);
    }
    free(database);
    database = NULL;
    dbSize = 0;
}

TVShow *findShow(char *name) {
    if (database == NULL || name == NULL) {
        return NULL;
    }
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL && strcmp(database[i][j]->name, name) == 0) {
                return database[i][j];
            }
        }
    }
    return NULL;
}
Season *findSeason(TVShow *show, char *name) {
    if (show == NULL) {
        return NULL;
    }
    Season *current = show->seasons;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
Episode *findEpisode(Season *season, char *name) {
    if (season == NULL || name == NULL) {
        return NULL;
    }
    Episode *current = season->episodes;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void addShow() {
    printf("Enter the name of the show:\n");
    char *name = getString();
//check duplicates
    if (findShow(name) != NULL) {
        printf("Show already exists.\n");
        free(name);
        return;
    }
    int numShows = countShows();
    if (database == NULL) {
        dbSize = 1;
        database = malloc(sizeof(TVShow**));
        database[0] = malloc(sizeof(TVShow*));
        database[0][0] = NULL;
}
//check if expansion is needed (if array is full)
    if (numShows >= dbSize * dbSize) {
        expandDB();
    }
    int insertPos = -1;
    for (int i = 0; i < dbSize * dbSize; i++) {
        int row = i / dbSize;
        int col = i % dbSize;
        if (database[row][col] == NULL) {
            insertPos = i;
            break;
        }
        if (strcmp(name, database[row][col]->name) < 0) {
            insertPos = i;
            break;
        }
    }
    for (int i = numShows; i > insertPos; i--) {
        int fromRow = (i - 1) / dbSize;
        int fromCol = (i - 1) % dbSize;
        int toRow = i / dbSize;
        int toCol = i % dbSize;
        database[toRow][toCol] = database[fromRow][fromCol];
    }
    TVShow *newShow = malloc(sizeof(TVShow));
    newShow->name = name;
    newShow->seasons = NULL;
    int row = insertPos / dbSize;
    int col = insertPos % dbSize;
    database[row][col] = newShow;
}
void addSeason() {
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    if (findSeason(show, seasonName) != NULL) {
        printf("Season already exists.\n");
        free(showName);
        free(seasonName);
        return;
    }
    Season *newSeason = malloc(sizeof(Season));
    newSeason->name = seasonName;
    newSeason->episodes = NULL;
    newSeason->next = NULL;
    printf("Enter the position:\n");
    int pos = getInt();
    if (show->seasons == NULL) {
        show->seasons = newSeason;
        free(showName);
        return;
    }
    if (pos == 0) {
        newSeason->next = show->seasons;
        show->seasons = newSeason;
        free(showName);
        return;
    }
    Season *current = show->seasons;
    int currentPos = 0;
    while (current ->next != NULL && currentPos < pos - 1) {
        current = current->next;
        currentPos++;
    }
    newSeason->next = current->next;
    current->next = newSeason;
}

void addEpisode() {
    char *showName = NULL;
    char *seasonName = NULL;
    char *episodeName = NULL;
    char *length = NULL;
    printf("Enter the name of the show:\n");
    showName = getString();
    TVShow *show = findShow(showName);
    if (!show) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    seasonName = getString();
    Season *season = findSeason(show, seasonName);
    if (!season) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }
    printf("Enter the name of the episode:\n");
    episodeName = getString();
    if (findEpisode(season, episodeName)) {
        printf("Episode already exists.\n");
        free(showName);
        free(seasonName);
        free(episodeName);
        return;
    }
    printf("Enter the length(xx:xx:xx):\n");
    length = getString();
    while (!validLength(length)) {
        printf("Invalid length, enter again:\n");
        free(length);
        length = getString();
    }
    Episode *newEpisode = malloc(sizeof(Episode));
    if (!newEpisode) {
        free(showName);
        free(seasonName);
        free(episodeName);
        free(length);
        return;
    }
    newEpisode->name = episodeName;
    newEpisode->length = length;
    newEpisode->next = NULL;
    printf("Enter the position:\n");
    int pos = getInt();
    if (pos == 0) {
        newEpisode->next = season->episodes;
        season->episodes = newEpisode;
        free(showName);
        free(seasonName);
        return;
    }
    Episode *current = season->episodes;
    int currentPos = 0;
    while (current->next != NULL && currentPos < pos - 1) {
        current = current->next;
        currentPos++;
    }
    newEpisode->next = current->next;
    current->next = newEpisode;
    free(showName);
    free(seasonName);
}
void deleteShow() {
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    int deletePos = -1;
    for (int i = 0; i < dbSize * dbSize; i++) {
        int row = i / dbSize;
        int col = i % dbSize;
        if (database[row][col] == show) {
            deletePos = i;
            break;
        }
    }
    freeShow(show);
    free(showName);
    int numShows = countShows();
    for (int i = deletePos; i < numShows; i++) {
        int fromRow = (i + 1) / dbSize;
        int fromCol = (i+1) % dbSize;
    int toRow = i / dbSize;
    int toCol = i % dbSize;
    database[toRow][toCol] = database[fromRow][fromCol];
    }
    int lastRow = numShows  / dbSize;
    int lastCol = numShows % dbSize;
    database[lastRow][lastCol] = NULL;
    if (numShows <= (dbSize -1) * (dbSize - 1)) {
        shrinkDB();
    }
}
void deleteSeason() {
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season *season = findSeason(show, seasonName);
    if (season == NULL) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }
    if (show->seasons == season) {
        show->seasons = season->next;
        freeSeason(season);
        free(showName);
        free(seasonName);
        return;
    }
    Season *prev = show->seasons;
    while (prev->next != season) {
        prev = prev->next;
    }
    prev->next = season->next;
    freeSeason(season);
}
void deleteEpisode() {
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season *season = findSeason(show, seasonName);
    if (season == NULL) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }
    printf("Enter the name of the episode:\n");
    char *episodeName = getString();
    Episode *episode = findEpisode(season, episodeName);
    if (episode == NULL) {
        printf("Episode not found.\n");
        free(showName);
        free(seasonName);
        free(episodeName);
        return;
    }
    if (season->episodes == episode) {
        season->episodes = episode->next;
        freeEpisode(episode);
        free(showName);
        free(seasonName);
        free(episodeName);
        return;
    }
    Episode *prev = season->episodes;
    while (prev->next != episode) {
        prev = prev->next;
    }
    prev->next = episode->next;
    freeEpisode(episode);
}

void printEpisode() {
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season *season = findSeason(show, seasonName);
    if (season == NULL) {
        printf("Season not found.\n");
        free(showName);
        free(seasonName);
        return;
    }
    printf("Enter the name of the episode:\n");
    char *episodeName = getString();
    Episode *episode = findEpisode(season, episodeName);
    if (episode == NULL) {
        printf("Episode not found.\n");
        free(showName);
        free(seasonName);
        free(episodeName);
        return;
    }
    printf("Name: %s\n", episode->name);
    printf("Length: %s\n", episode->length);
    free(showName);
    free(seasonName);
    free(episodeName);
}

void printShow() {
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow *show = findShow(showName);
    if (show == NULL) {
        printf("Show not found.\n");
        free(showName);
        return;
    }
    printf("Name: %s\n", show->name);
    printf("Seasons:\n");
    Season *season = show->seasons;
    int seasonIndex = 0;
    while (season != NULL) {
        printf("\tSeason %d: %s\n", seasonIndex ,season->name);
        Episode *episode = season->episodes;
        int episodeIndex = 0;
        while (episode != NULL) {
            printf("\t\tEpisode %d: %s (%s)\n", episodeIndex, episode->name, episode->length);
            episode = episode->next;
            episodeIndex++;
        }
        season = season->next;
        seasonIndex++;
    }
    free(showName);
}
void printArray() {
    if (database == NULL || dbSize == 0) {
        return;
    }
    for (int i = 0; i < dbSize; i++) {
        for (int j = 0; j < dbSize; j++) {
            if (database[i][j] != NULL) {
                printf("[%s]", database[i][j]->name);
            } else {
                printf("[NULL]");
            }
        }
        printf("\n");
    }
}
    int main() {
        int choice;
        do {
            mainMenu();
            scanf("%d", &choice);
            getchar();
            switch (choice) {
                case 1: addMenu(); break;
                case 2: deleteMenu(); break;
                case 3: printMenuSub(); break;
                case 4: freeAll(); break;
            }
        } while (choice != 4);
        return 0;
    }
