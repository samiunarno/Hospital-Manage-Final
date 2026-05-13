#include "../include/auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 500
#define USERS_FILE "data/users.txt"

static User users[MAX_USERS];
static int user_count = 0;

// Simple string hashing function (djb2) for basic "data protection"
static void hash_password(const char *password, char *output) {
    unsigned long hash = 5381;
    int c;
    while ((c = *password++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    snprintf(output, MAX_PASSWORD_LEN, "%lx", hash);
}

void auth_init(void) {
    FILE *fp = fopen(USERS_FILE, "r");
    if (fp) {
        if (fscanf(fp, "%d\n", &user_count) == 1) {
            for (int i = 0; i < user_count; i++) {
                char line[512];
                if (fgets(line, sizeof(line), fp)) {
                    line[strcspn(line, "\n")] = 0;
                    char *username = strtok(line, "|");
                    char *hash = strtok(NULL, "|");
                    char *fname = strtok(NULL, "|");
                    char *nid = strtok(NULL, "|");
                    char *role_str = strtok(NULL, "|");
                    char *app_str = strtok(NULL, "|");
                    
                    if (username && hash && fname && nid && role_str && app_str) {
                        strncpy(users[i].username, username, MAX_USERNAME_LEN-1);
                        strncpy(users[i].password_hash, hash, MAX_PASSWORD_LEN-1);
                        strncpy(users[i].full_name, fname, MAX_NAME_LEN-1);
                        strncpy(users[i].national_id, nid, MAX_NID_LEN-1);
                        users[i].role = (UserRole)atoi(role_str);
                        users[i].is_approved = atoi(app_str) ? true : false;
                    }
                }
            }
        }
        fclose(fp);
    } 
    
    if (user_count == 0) {
        // Create a default admin if file doesn't exist
        User *admin = &users[user_count];
        strncpy(admin->username, "admin", MAX_USERNAME_LEN - 1);
        admin->username[MAX_USERNAME_LEN - 1] = '\0';
        
        hash_password("admin123", admin->password_hash);
        
        strncpy(admin->full_name, "System Administrator", MAX_NAME_LEN - 1);
        admin->full_name[MAX_NAME_LEN - 1] = '\0';
        
        strncpy(admin->national_id, "ADMIN-000000", MAX_NID_LEN - 1);
        admin->national_id[MAX_NID_LEN - 1] = '\0';
        
        admin->role = ROLE_ADMIN;
        admin->is_approved = true; // Admin is auto-approved
        user_count++;
        
        auth_cleanup(); // Save immediately
    }
}

void auth_cleanup(void) {
    // Create data directory if it doesn't exist
    FILE *fp = fopen(USERS_FILE, "w");
    if (fp) {
        fprintf(fp, "%d\n", user_count);
        for (int i = 0; i < user_count; i++) {
            fprintf(fp, "%s|%s|%s|%s|%d|%d\n",
                    users[i].username,
                    users[i].password_hash,
                    users[i].full_name,
                    users[i].national_id,
                    users[i].role,
                    users[i].is_approved ? 1 : 0);
        }
        fclose(fp);
    }
}

bool auth_register(const char *username, const char *password, 
                   const char *full_name, const char *national_id, UserRole role) {
    if (user_count >= MAX_USERS) return false;

    // Validate inputs
    if (!username || !password || !full_name || !national_id) return false;
    if (strlen(username) == 0 || strlen(password) == 0 || 
        strlen(full_name) == 0 || strlen(national_id) == 0) {
        return false;
    }

    // Check if username already exists
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return false;
        }
    }

    // Check if national ID already exists
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].national_id, national_id) == 0) {
            return false;
        }
    }

    User *new_user = &users[user_count];
    strncpy(new_user->username, username, MAX_USERNAME_LEN - 1);
    new_user->username[MAX_USERNAME_LEN - 1] = '\0';

    hash_password(password, new_user->password_hash);

    strncpy(new_user->full_name, full_name, MAX_NAME_LEN - 1);
    new_user->full_name[MAX_NAME_LEN - 1] = '\0';

    strncpy(new_user->national_id, national_id, MAX_NID_LEN - 1);
    new_user->national_id[MAX_NID_LEN - 1] = '\0';

    new_user->role = role;
    new_user->is_approved = false; // Requires admin approval

    user_count++;
    auth_cleanup(); // Save immediately to users.txt
    return true;
}

User* auth_login(const char *username, const char *password) {
    if (!username || !password) return NULL;
    
    char hashed[MAX_PASSWORD_LEN];
    hash_password(password, hashed);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && 
            strcmp(users[i].password_hash, hashed) == 0) {
            if (users[i].is_approved) {
                return &users[i];
            }
        }
    }
    return NULL;
}

bool auth_approve_user(const char *username) {
    if (!username) return false;
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && !users[i].is_approved) {
            users[i].is_approved = true;
            auth_cleanup(); // Save changes
            return true;
        }
    }
    return false;
}

bool auth_reject_user(const char *username) {
    if (!username) return false;
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && !users[i].is_approved) {
            // Shift remaining users
            for (int j = i; j < user_count - 1; j++) {
                users[j] = users[j + 1];
            }
            user_count--;
            auth_cleanup();
            return true;
        }
    }
    return false;
}

bool auth_get_pending_users(User *pending_users, int *count) {
    if (!pending_users || !count) return false;
    
    *count = 0;
    for (int i = 0; i < user_count; i++) {
        if (!users[i].is_approved) {
            pending_users[*count] = users[i];
            (*count)++;
        }
    }
    return true;
}

bool auth_get_approved_users(User *approved_users, int *count) {
    if (!approved_users || !count) return false;
    
    *count = 0;
    for (int i = 0; i < user_count; i++) {
        if (users[i].is_approved) {
            approved_users[*count] = users[i];
            (*count)++;
        }
    }
    return true;
}

int auth_approve_all_pending(void) {
    int approved_count = 0;
    for (int i = 0; i < user_count; i++) {
        if (!users[i].is_approved) {
            users[i].is_approved = true;
            approved_count++;
        }
    }
    if (approved_count > 0) {
        auth_cleanup();
    }
    return approved_count;
}

bool auth_delete_user(const char *username) {
    if (!username) return false;
    
    // Prevent deleting the last admin
    int admin_count = 0;
    for (int i = 0; i < user_count; i++) {
        if (users[i].role == ROLE_ADMIN && users[i].is_approved) {
            admin_count++;
        }
    }
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            // Don't delete if it's the only admin
            if (users[i].role == ROLE_ADMIN && admin_count <= 1) {
                return false;
            }
            
            // Shift remaining users
            for (int j = i; j < user_count - 1; j++) {
                users[j] = users[j + 1];
            }
            user_count--;
            auth_cleanup();
            return true;
        }
    }
    return false;
}

bool auth_update_user_role(const char *username, UserRole new_role) {
    if (!username) return false;
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && users[i].is_approved) {
            users[i].role = new_role;
            auth_cleanup();
            return true;
        }
    }
    return false;
}

bool auth_user_exists(const char *username) {
    if (!username) return false;
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return true;
        }
    }
    return false;
}

User* auth_get_user(const char *username) {
    if (!username) return NULL;
    
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

const char* auth_role_to_string(UserRole role) {
    switch(role) {
        case ROLE_ADMIN: return "Admin";
        case ROLE_RECEPTIONIST: return "Receptionist";
        case ROLE_DOCTOR: return "Doctor";
        case ROLE_PATIENT: return "Patient";
        default: return "Unknown";
    }
}

UserRole auth_string_to_role(const char *role_str) {
    if (strcmp(role_str, "Admin") == 0) return ROLE_ADMIN;
    if (strcmp(role_str, "Receptionist") == 0) return ROLE_RECEPTIONIST;
    if (strcmp(role_str, "Doctor") == 0) return ROLE_DOCTOR;
    if (strcmp(role_str, "Patient") == 0) return ROLE_PATIENT;
    return ROLE_PATIENT; // Default
}

int auth_get_user_count(void) {
    return user_count;
}

int auth_get_pending_count(void) {
    int count = 0;
    for (int i = 0; i < user_count; i++) {
        if (!users[i].is_approved) {
            count++;
        }
    }
    return count;
}