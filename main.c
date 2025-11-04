#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userFuncs.h"

void addUser(struct user *users, int *userCount);
void updateMemory(struct user *users, int *userCount);
int getUserCount();

int main(){
    //init memory allocation for num of users
    int userCount = getUserCount(); //get user count
    struct user *users = malloc(userCount * sizeof(struct user));
    if (users == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    updateMemory(users, &userCount);

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

int getUserCount(){
    char line[256];
    int userCount = 0;

    FILE *file = fopen("users.csv", "r");
    if (!file) {
        printf("Could not open file.\n");
        return 1;
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
    char line[256];

    printf("\nAdd new user:\n");
    printf("Enter your username: ");
    scanf("%99s", name);

    printf("Enter your password: ");
    scanf("%99s", pass);

    FILE *file = fopen("users.csv", "a");
    fgets(line, sizeof(line), file);
    if (!file) {
        printf("Error opening CSV for writing.\n");
        return; 
    }

    fprintf(file, "\n%s,%s\n", name, pass);
    fclose(file);

    // add new user to memory as well
    users[*userCount] = updateUsers(name, pass, *userCount + 1);
    (*userCount)++;

    printf("User '%s' added successfully!\n", name);
}

void updateMemory(struct user *users, int *userCount){
    //open the file and go through csv file and create users for each
    FILE *file = fopen("users.csv", "r");
    if (!file) return;

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
}