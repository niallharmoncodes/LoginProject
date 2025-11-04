#ifndef USERFUNCS_H
#define USERFUNCS_H

struct user {
    char *username;
    char *password;
    int id;
};

struct user createUser(const char *name, const char *password, int id);

void freeUser(struct user *u);

//void openUserFile();

#endif