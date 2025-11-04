#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userFuncs.h"

void getUserInfo();
int getUserCount();

int main(){
    int userCount = getUserCount();

    struct user *users = malloc(userCount * sizeof(struct user));
    if (users == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    

    FILE *file = fopen("users.csv", "r");
    if (!file) return 1;

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // remove newline
        char *username = strtok(line, ",");
        char *password = strtok(NULL, ",");

        if (username && password) {
            users[count++] = createUser(username, password, count);
        }
    }

    fclose(file);

    // Print users
    for (int i = 0; i < count; i++) {
        printf("%d: %s / %s\n", users[i].id, users[i].username, users[i].password);
        freeUser(&users[i]);
    }
    return 0;
}

void getUserInfo(){
    char username[100];
    char password[100];

    printf("Enter username: ");
    scanf("%99s", username);  // read up to 99 chars, stops at whitespace

    printf("Enter password: ");
    scanf("%99s", password);

    struct user u = createUser(username, password, 1);

    freeUser(&u);
}

int getUserCount(){
    FILE *file = fopen("users.csv", "r");
    if (!file) {
        printf("Could not open file.\n");
        return 1;
    }

    char line[256];
    int userCount = 0;

    // If your CSV has a header row, skip it
    if (fgets(line, sizeof(line), file)) {
        // header skipped
    }

    // Count remaining lines
    while (fgets(line, sizeof(line), file)) {
        // Optional: skip empty lines
        if (line[0] != '\n') {
            userCount++;
        }
    }

    fclose(file);

    return userCount;
}