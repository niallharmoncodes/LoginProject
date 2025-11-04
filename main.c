#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userFuncs.h"

void getUserInfo();
void addUser(struct user *users, int *userCount);
int getUserCount();

int main(){
    //init memory allocation for num of users
    int userCount = getUserCount(); //get user count
    struct user *users = malloc(userCount * sizeof(struct user));
    if (users == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    //open the file and go through csv file and create users for each
    FILE *file = fopen("users.csv", "r");
    if (!file) return 1;

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // remove newline
        char *username = strtok(line, ",");
        char *password = strtok(NULL, ",");

        if (username && password) {
            users[count++] = updateUsers(username, password, count);
        }
    }

    fclose(file);

    int running = 1;
    while (running)
    {
        int choice = 0;
        printf("1: Login\n2: Create New User\n3: See User List\n");
        scanf("%d", &choice);

        switch(choice){
            case 1:
                break;
            case 2:
                addUser(users, &userCount);
                break;
            case 3:
                printf("\nLoaded users:\n");
                for (int i = 0; i < getUserCount(); i++) {
                    printf("%d: %s / %s\n", users[i].id, users[i].username, users[i].password);
                }
                printf("\n");
                break;
            default:
                printf("Sorry! That's not on option...\n");
        }
    }
    
}

void getUserInfo(){
    char username[100];
    char password[100];

    printf("Enter username: ");
    scanf("%99s", username);  // read up to 99 chars, stops at whitespace

    printf("Enter password: ");
    scanf("%99s", password);

    struct user u = updateUsers(username, password, 1);

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

void addUser(struct user *users, int *userCount) {
    char name[100];
    char pass[100];

    printf("\nAdd new user:\n");
    printf("Enter your username: ");
    scanf("%99s", name);

    printf("Enter your password: ");
    scanf("%99s", pass);

    FILE *file = fopen("users.csv", "a");
    if (!file) {
        printf("Error opening CSV for writing.\n");
        return; 
    }

    fprintf(file, "%s,%s\n", name, pass);
    fclose(file);

    // add new user to memory as well
    users[*userCount] = updateUsers(name, pass, *userCount + 1);
    (*userCount)++;

    printf("User '%s' added successfully!\n", name);
}