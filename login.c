#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_ATTEMPTS 3

void
clearline(void)
{
    printf(1, "\r                                                  \r");
}

int
main(void)
{
    char username[16];
    char password[16];
    int attempts = 0;
    int i;
    
    printf(1, "\n");
    printf(1, "================================\n");
    printf(1, "    Welcome to xv6 OS Login    \n");
    printf(1, "================================\n");
    printf(1, "\n");
    
    while(attempts < MAX_ATTEMPTS) {
        // Get username
        printf(1, "Username: ");
        memset(username, 0, sizeof(username));
        gets(username, sizeof(username));
        
        // Remove newline
        for(i = 0; username[i] != '\n' && username[i] != '\0'; i++);
        username[i] = '\0';
        
        // Get password
        printf(1, "Password: ");
        memset(password, 0, sizeof(password));
        gets(password, sizeof(password));
        
        // Remove newline
        for(i = 0; password[i] != '\n' && password[i] != '\0'; i++);
        password[i] = '\0';
        
        // Authenticate
        if(authenticate(username, password) == 0) {
            printf(1, "\nLogin successful! Welcome, %s\n\n", username);
            
            // Start shell
            char *argv[] = { "sh", 0 };
            exec("sh", argv);
            printf(1, "exec sh failed\n");
            exit();
        }
        
        attempts++;
        printf(1, "\nAuthentication failed. ");
        if(attempts < MAX_ATTEMPTS) {
            printf(1, "Attempts remaining: %d\n\n", MAX_ATTEMPTS - attempts);
        }
    }
    
    printf(1, "\nMaximum login attempts exceeded.\n");
    printf(1, "System locked. Please restart.\n");
    
    // Hang the system
    while(1) {
        sleep(100);
    }
    
    exit();
}
