#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>       
#include <windows.h>     
#include <sys/types.h>
#include <sys/stat.h>

#define MAX 100

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

void displayUserMenu();
int isValidPassword(const char *password);
void getPasswordInput(char *password);
void viewMetadata(const char *filename);
void modifyFile();
void enableANSIColors();
int registerUser();
int loginUser();

int main() {
    enableANSIColors();

    int authenticated = 0;
    while (1) {
        while (!authenticated) {
            printf(YELLOW "\n========== Secure File Management ==========\n" RESET);
            printf(YELLOW "1. Register\n2. Login\n3. Exit\nChoose option: " RESET);
            int option;
            scanf("%d", &option);
            getchar();

            if (option == 1) {
                if (registerUser()) {
                    printf(GREEN "\n✔ Registration successful! Please login.\n" RESET);
                }
            } else if (option == 2) {
                if (loginUser()) {
                    printf(GREEN "\n✔ Login successful!\n" RESET);
                    authenticated = 1;
                } else {
                    printf(RED "\n✘ Login failed. Try again.\n" RESET);
                }
            } else if (option == 3) {
                printf(GREEN "\n✔ Exiting program. Goodbye!\n" RESET);
                exit(0);
            } else {
                printf(RED "\n✘ Invalid option. Try again.\n" RESET);
            }
        }

        int choice;
        char filename[MAX];

        while (authenticated) {
            displayUserMenu();
            printf(YELLOW "\nEnter your choice: " RESET);
            scanf("%d", &choice);
            getchar();

            switch (choice) {
                case 1: {
                    printf(CYAN "\nEnter file name to create (with extension, e.g., 'file.txt'): " RESET);
                    fgets(filename, MAX, stdin);
                    filename[strcspn(filename, "\n")] = 0;

                    if (strchr(filename, '.') == NULL) {
                        strcat(filename, ".txt");
                    }

                    FILE *f = fopen(filename, "w");
                    if (f) {
                        printf(MAGENTA "Enter content: " RESET);
                        char content[MAX];
                        fgets(content, MAX, stdin);
                        fputs(content, f);
                        fclose(f);
                        printf(GREEN "\n✔ File '%s' created successfully!\n" RESET, filename);
                    } else {
                        printf(RED "\n✘ Failed to open file for writing.\n" RESET);
                    }
                    break;
                }

                case 2: {
                    printf(CYAN "\nEnter file name to read: " RESET);
                    fgets(filename, MAX, stdin);
                    filename[strcspn(filename, "\n")] = 0;

                    FILE *fr = fopen(filename, "r");
                    if (fr) {
                        printf("\n" YELLOW "--- File Content ---\n" RESET);
                        char ch;
                        while ((ch = fgetc(fr)) != EOF) {
                            putchar(ch);
                        }
                        fclose(fr);
                        printf("\n" GREEN "--- End of File ---\n" RESET);
                    } else {
                        printf(RED "\n✘ File not found.\n" RESET);
                    }
                    break;
                }

                case 3: {
                    printf(CYAN "\nEnter file name to view metadata: " RESET);
                    fgets(filename, MAX, stdin);
                    filename[strcspn(filename, "\n")] = 0;
                    viewMetadata(filename);
                    break;
                }

                case 4: {
                    printf(GREEN "\n✔ Logged out. Returning to main menu...\n" RESET);
                    authenticated = 0;
                    break;
                }

                case 5:
                    modifyFile();
                    break;

                default:
                    printf(RED "\n✘ Invalid choice. Please try again.\n" RESET);
            }
        }
    }

    return 0;
}

void displayUserMenu() {
    printf(CYAN "\n========== " YELLOW "Secure File Management Menu" CYAN " ==========\n" RESET);
    printf(GREEN "1. " RESET "Write File\n");
    printf(GREEN "2. " RESET "Read File\n");
    printf(GREEN "3. " RESET "View Metadata\n");
    printf(GREEN "4. " RESET "Logout\n");
    printf(GREEN "5. " RESET "Modify File\n");
    printf(CYAN "===========================================\n" RESET);
}

int isValidPassword(const char *password) {
    if (strlen(password) < 8) return 0;
    int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
    for (int i = 0; password[i]; i++) {
        if (isupper(password[i])) hasUpper = 1;
        else if (islower(password[i])) hasLower = 1;
        else if (isdigit(password[i])) hasDigit = 1;
        else hasSpecial = 1;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

void getPasswordInput(char *password) {
    int i = 0;
    char ch;
    while ((ch = _getch()) != '\r' && i < MAX - 1) {
        if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (isprint(ch)) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

void viewMetadata(const char *filename) {
    struct _stat64 info;
    if (_stat64(filename, &info) == 0) {
        const char *ext = strrchr(filename, '.');
        if (ext) {
            ext++;
        } else {
            ext = "No extension";
        }

        printf(YELLOW "\n--- File Metadata ---\n" RESET);
        printf(BLUE "Name: " RESET "%s\n", filename);
        printf(BLUE "Type: " RESET "%s\n", ext);
        printf(BLUE "Size: " RESET "%lld bytes\n", info.st_size);
        printf(BLUE "Last Modified: " RESET "%s", ctime(&info.st_mtime));
        printf(BLUE "Last Accessed: " RESET "%s", ctime(&info.st_atime));
    } else {
        printf(RED "\n✘ Error retrieving metadata.\n" RESET);
    }
}

void modifyFile() {
    char oldName[MAX], newName[MAX], newContent[MAX];
    printf(CYAN "\nEnter existing file name: " RESET);
    fgets(oldName, MAX, stdin);
    oldName[strcspn(oldName, "\n")] = 0;

    FILE *fp = fopen(oldName, "r");
    if (!fp) {
        printf(RED "\n✘ File not found!\n" RESET);
        return;
    }
    fclose(fp);

    printf(YELLOW "Enter new filename (or press enter to skip): " RESET);
    fgets(newName, MAX, stdin);
    newName[strcspn(newName, "\n")] = 0;

    printf(YELLOW "Enter new content (or press enter to skip): " RESET);
    fgets(newContent, MAX, stdin);
    newContent[strcspn(newContent, "\n")] = 0;

    if (strlen(newName) > 0) {
        rename(oldName, newName);
        strcpy(oldName, newName);
    }

    if (strlen(newContent) > 0) {
        FILE *fw = fopen(oldName, "w");
        if (fw) {
            fputs(newContent, fw);
            fclose(fw);
            printf(GREEN "\n✔ File modified successfully.\n" RESET);
        } else {
            printf(RED "\n✘ Failed to write content.\n" RESET);
        }
    }
}

void enableANSIColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

int registerUser() {
    char uname[MAX], pwd[MAX];
    printf(YELLOW "\n--- Register New User ---\n" RESET);
    printf(YELLOW "Enter username: " RESET);
    fgets(uname, MAX, stdin);
    uname[strcspn(uname, "\n")] = 0;

    while (1) {
        printf(YELLOW "Enter password (min 8 chars): " RESET);
        getPasswordInput(pwd);
        if (isValidPassword(pwd)) {
            FILE *f = fopen("users.txt", "a");
            if (f) {
                fprintf(f, "%s %s\n", uname, pwd);
                fclose(f);
                return 1;
            } else {
                printf(RED "\n✘ Error saving user.\n" RESET);
                return 0;
            }
        } else {
            printf(RED "\n✘ Password must include uppercase, lowercase, digit, and special character.\n" RESET);
        }
    }
}

int loginUser() {
    char uname[MAX], pwd[MAX], fileU[MAX], fileP[MAX];
    printf(YELLOW "\n--- Login ---\n" RESET);
    printf(YELLOW "Enter username: " RESET);
    fgets(uname, MAX, stdin);
    uname[strcspn(uname, "\n")] = 0;

    printf(YELLOW "Enter password: " RESET);
    getPasswordInput(pwd);

    FILE *f = fopen("users.txt", "r");
    if (!f) return 0;

    while (fscanf(f, "%s %s", fileU, fileP) != EOF) {
        if (strcmp(fileU, uname) == 0 && strcmp(fileP, pwd) == 0) {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}
