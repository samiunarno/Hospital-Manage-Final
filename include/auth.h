#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>

#define MAX_USERNAME_LEN 50
#define MAX_PASSWORD_LEN 64 // Storing hash
#define MAX_NAME_LEN 100
#define MAX_NID_LEN 50

typedef enum {
    ROLE_ADMIN,
    ROLE_RECEPTIONIST,
    ROLE_DOCTOR,
    ROLE_PATIENT,
    ROLE_UNKNOWN
} UserRole;

typedef struct {
    char username[MAX_USERNAME_LEN];
    char password_hash[MAX_PASSWORD_LEN];
    char full_name[MAX_NAME_LEN];
    char national_id[MAX_NID_LEN];
    UserRole role;
    bool is_approved;
} User;

// Initialize auth system (load data)
void auth_init(void);

// Save auth system (save data)
void auth_cleanup(void);

// Register a new user
// Real name and National ID are required.
bool auth_register(const char *username, const char *password, const char *full_name, const char *national_id, UserRole role);

// Login a user, returns the logged in user or NULL if failed
User* auth_login(const char *username, const char *password);

// Get role as string
const char* auth_role_to_string(UserRole role);

int auth_get_pending_count(void);
bool auth_get_pending_users(User *pending_users, int *count);
bool auth_approve_user(const char *username);
bool auth_reject_user(const char *username);
bool auth_get_approved_users(User *approved_users, int *count);
int auth_approve_all_pending(void);
bool auth_delete_user(const char *username);
bool auth_update_user_role(const char *username, UserRole new_role);
bool auth_user_exists(const char *username);
User* auth_get_user(const char *username);
UserRole auth_string_to_role(const char *role_str);
int auth_get_user_count(void);

#endif // AUTH_H
