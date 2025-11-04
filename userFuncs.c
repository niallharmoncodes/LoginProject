#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userFuncs.h"

struct user updateUsers(const char *name, const char *password, int id){
    struct user u;
    u.id = id;
    u.username = malloc(strlen(name) + 1);
    if (u.username == NULL) {
        return u;
    }
    strcpy(u.username, name);

    u.password = malloc(strlen(password) + 1);
    if (u.password == NULL) {
        free(u.username);
        return u; 
    }
    strcpy(u.password, password);
    return u;
}


void freeUser(struct user *u) {
    free(u->username);
    free(u->password);
}

