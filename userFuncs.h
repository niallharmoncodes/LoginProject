#ifndef USERFUNCS_H
#define USERFUNCS_H

struct user {
    char *username;
    char *password;
    int id;
};

struct user updateUsers(const char *name, const char *password, int id);

#endif