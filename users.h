// users.h
#ifndef USERS_H
#define USERS_H

struct user {
    char username[16];
    char password[16];
};

// Simple user database (in real systems, passwords would be hashed)
static struct user users[] = {
    {"root", "root123"},
    {"admin", "admin123"},
    {"user", "password"}
};

#define NUMUSERS (sizeof(users)/sizeof(users[0]))

#endif
