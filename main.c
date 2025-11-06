// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userFuncs.h"

#define ADMIN_PASS "adminpassword"
#define CSV_PATH "users.csv"

void addUser(struct user *users, int *userCount, int *isEncrypted);
void updateMemory(struct user *users, int *userCount, int *isEncrypted);
void encryptPass(struct user *users, int *userCount, int *isEncrypted);
int getUserCountFromFile();
void writeAllToCSV(struct user *users, int userCount, int isEncrypted);

void xor_encrypt(char *data, char key) {
    if (!data) return;
    size_t len = strlen(data);
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

int main() {
    int userCount = getUserCountFromFile();
    // ensure at least room for new users
    if (userCount < 1) userCount = 1;

    struct user *users = malloc((userCount + 5) * sizeof(struct user)); // slack room
    if (!users) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    int isEncrypted = 0; // 0 = plain, 1 = encrypted
    // load users and detect encryption state
    updateMemory(users, &userCount, &isEncrypted);

    int running = 1;
    while (running) {
        int choice = 0;
        printf("\n=== User System Menu ===\n");
        printf("1: Login (not implemented)\n");
        printf("2: Create New User\n");
        printf("3: See User List\n");
        printf("4: Encrypt/Decrypt Passwords (Admin Only)\n");
        printf("5: Quit\n> ");
        if (scanf("%d", &choice) != 1) {
            // clear bad input
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        switch (choice) {
            case 1:
                // login(users, userCount);
                printf("Login not implemented in this snippet.\n");
                break;

            case 2:
                addUser(users, &userCount, &isEncrypted);
                break;

            case 3:
                printf("\nLoaded users:\n");
                for (int i = 0; i < userCount; i++) {
                    printf("%d: %s / %s\n", users[i].id, users[i].username, users[i].password);
                }
                printf("Current CSV state: %s\n", isEncrypted ? "ENCRYPTED" : "PLAINTEXT");
                break;

            case 4: {
                char adminInput[128];
                printf("\nEnter admin password to continue: ");
                scanf("%127s", adminInput);

                if (strcmp(adminInput, ADMIN_PASS) == 0) {
                    printf("Access granted. Toggling encryption and updating CSV...\n");
                    encryptPass(users, &userCount, &isEncrypted);
                    printf("CSV file successfully updated! (Now %s)\n",
                           isEncrypted ? "ENCRYPTED" : "DECRYPTED");
                } else {
                    printf("Access denied. Incorrect admin password.\n");
                }
                break;
            }

            case 5:
                running = 0;
                break;

            default:
                printf("Sorry! That's not an option...\n");
                break;
        }
    }

    free(users);
    return 0;
}

/* Count lines that look like user lines (username,password) ignoring optional header */
int getUserCountFromFile() {
    FILE *file = fopen(CSV_PATH, "r");
    if (!file) {
        // no file yet
        return 0;
    }

    char line[512];
    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        // skip header markers starting with '#'
        if (line[0] == '#') continue;
        // skip blank lines
        if (line[0] == '\n' || line[0] == '\0') continue;
        if (strchr(line, ',')) count++;
    }

    fclose(file);
    return count;
}

/* Loads users into memory and sets isEncrypted based on header (if present).
   Assumes updateUsers(name, pass, id) returns a struct user.
*/
void updateMemory(struct user *users, int *userCount, int *isEncrypted) {
    FILE *file = fopen(CSV_PATH, "r");
    if (!file) {
        // nothing to load
        *userCount = 0;
        *isEncrypted = 0;
        return;
    }

    char line[512];
    int count = 0;
    *isEncrypted = 0; // default

    // Read possible header first
    long pos = ftell(file);
    if (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') {
            // header found
            if (strstr(line, "ENCRYPTED")) *isEncrypted = 1;
            else *isEncrypted = 0;
        } else {
            // no header: rewind and treat this line as first user
            fseek(file, pos, SEEK_SET);
        }
    }

    while (fgets(line, sizeof(line), file)) {
        // trim newline
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;
        if (line[0] == '#') continue; // skip any stray header lines
        char *username = strtok(line, ",");
        char *password = strtok(NULL, ",");
        if (username && password) {
            // ensure the strings are duplicated / made into proper buffers for the struct
            // Assuming struct user has char username[...], char password[...], int id
            // We'll call updateUsers to build struct.
            // Set id = count + 1 (1-based)
            users[count] = updateUsers(username, password, count + 1);
            count++;
        }
    }

    fclose(file);
    *userCount = count;
}

/* Writes the header + all users to the CSV (overwrites file) */
void writeAllToCSV(struct user *users, int userCount, int isEncrypted) {
    FILE *file = fopen(CSV_PATH, "w");
    if (!file) {
        printf("Error opening CSV for write.\n");
        return;
    }
    if (isEncrypted)
        fprintf(file, "#ENCRYPTED\n");
    else
        fprintf(file, "#PLAIN\n");

    for (int i = 0; i < userCount; i++) {
        // ensure newline separated, no extra spaces
        fprintf(file, "%s,%s\n", users[i].username, users[i].password);
    }

    fclose(file);
}

void addUser(struct user *users, int *userCount, int *isEncrypted) {
    char name[128];
    char pass[128];

    printf("\nAdd new user:\n");
    printf("Enter username: ");
    scanf("%127s", name);

    printf("Enter password: ");
    scanf("%127s", pass);

    // Determine id for new user (1-based)
    int newId = (*userCount) + 1;

    // If system currently encrypted, encrypt the incoming password before writing
    if (*isEncrypted) {
        xor_encrypt(pass, (char)newId);
    }

    FILE *file = fopen(CSV_PATH, "r+");
    if (!file) {
        // file doesn't exist -> create with header + line
        file = fopen(CSV_PATH, "w");
        if (!file) {
            printf("Error opening CSV for writing.\n");
            return;
        }
        if (*isEncrypted) fprintf(file, "#ENCRYPTED\n");
        else fprintf(file, "#PLAIN\n");
        fprintf(file, "%s,%s\n", name, pass);
        fclose(file);
    } else {
        // file exists. Check first char for header.
        long pos = ftell(file);
        char firstLine[512];
        if (fgets(firstLine, sizeof(firstLine), file)) {
            if (firstLine[0] != '#') {
                rewind(file);
                char rest[4096];
                size_t readBytes = fread(rest, 1, sizeof(rest) - 1, file);
                rest[readBytes] = '\0';
                fclose(file);
                // rewrite with header
                file = fopen(CSV_PATH, "w");
                if (!file) {
                    printf("Error writing CSV.\n");
                    return;
                }
                if (*isEncrypted) fprintf(file, "#ENCRYPTED\n");
                else fprintf(file, "#PLAIN\n");
                // write old contents (which were plain user lines)
                if (readBytes > 0) {
                    fprintf(file, "%s", rest);
                    // ensure file ends with newline before appending
                    if (rest[readBytes - 1] != '\n') fprintf(file, "\n");
                }
                // append new user
                fprintf(file, "%s,%s\n", name, pass);
                fclose(file);
            } else {
                // header exists -> append to file
                fclose(file);
                file = fopen(CSV_PATH, "a");
                if (!file) {
                    printf("Error opening CSV to append.\n");
                    return;
                }
                fprintf(file, "%s,%s\n", name, pass);
                fclose(file);
            }
        } else {
            // empty file, just write header and user
            fclose(file);
            file = fopen(CSV_PATH, "w");
            if (!file) {
                printf("Error opening CSV for writing.\n");
                return;
            }
            if (*isEncrypted) fprintf(file, "#ENCRYPTED\n");
            else fprintf(file, "#PLAIN\n");
            fprintf(file, "%s,%s\n", name, pass);
            fclose(file);
        }
    }

    // Update memory: create struct and add to users array
    users[*userCount] = updateUsers(name, pass, newId);
    (*userCount)++;

    printf("User '%s' added successfully! (%s)\n", name, *isEncrypted ? "Encrypted" : "Plaintext");
}

/* Toggle encryption for all users, write to CSV, flip isEncrypted */
void encryptPass(struct user *users, int *userCount, int *isEncrypted) {
    // XOR each password using the user's id as key (cast to char)
    for (int i = 0; i < *userCount; i++) {
        xor_encrypt(users[i].password, (char)users[i].id);
    }

    // flip flag
    *isEncrypted = !(*isEncrypted);

    // write updated file with header
    writeAllToCSV(users, *userCount, *isEncrypted);
}
