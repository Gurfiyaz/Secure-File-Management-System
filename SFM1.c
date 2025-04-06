#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <conio.h>  // For _getch() on Windows


#define MAX 100

// ANSI color codes
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define CYAN    "\x1b[36m"
#define YELLOW  "\x1b[33m"
#define MAGENTA "\x1b[35m"
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"

// Function Prototypes
void displayUserMenu();
int isValidPassword(const char *password);
void getPasswordInput(char *password);
void viewMetadata(const char *filename);
void modifyFile();
void uploadBinaryFile();

int main() {
    int choice;
    char filename[MAX];

    while (1) {
        displayUserMenu();
        printf(YELLOW "\nEnter your choice: " RESET);
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1: {
                printf(CYAN "\nEnter file name to create: " RESET);
                fgets(filename, MAX, stdin);
                filename[strcspn(filename, "\n")] = 0;

                FILE *f = fopen(filename, "w");
                if (f) {
                    printf(MAGENTA "Enter content: " RESET);
                    char content[MAX];
                    fgets(content, MAX, stdin);
                    fputs(content, f);
                    fclose(f);
                    printf(GREEN "\n✔ File saved successfully!\n" RESET);
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

            case 4:
                printf(GREEN "\n✔ Logged out. Goodbye!\n" RESET);
                exit(0);

            case 5: {
                printf(CYAN "\n--- Register New User ---\n" RESET);
                char uname[MAX], pwd[MAX];
                printf(YELLOW "Enter username: " RESET);
                fgets(uname, MAX, stdin);
                uname[strcspn(uname, "\n")] = 0;

                while (1) {
                    printf(YELLOW "Enter password (min 8 chars): " RESET);
                    getPasswordInput(pwd);
                    if (isValidPassword(pwd)) {
                        printf(GREEN "\n✔ Password set successfully!\n" RESET);
                        break;
                    } else {
                        printf(RED "\n✘ Password must include uppercase, lowercase, digit, and special character.\n" RESET);
                    }
                }
                break;
            }

            case 6:
                modifyFile();
                break;

            case 7:
                uploadBinaryFile();
                break;

            default:
                printf(RED "\n✘ Invalid choice. Please try again.\n" RESET);
        }
    }

    return 0;
}

void displayUserMenu() {
    printf(CYAN "\n========== " YELLOW "Secure File Management" CYAN " ==========\n" RESET);
    printf(GREEN "1. " RESET "Write File\n");
    printf(GREEN "2. " RESET "Read File\n");
    printf(GREEN "3. " RESET "View Metadata\n");
    printf(GREEN "4. " RESET "Logout\n");
    printf(GREEN "5. " RESET "Register New User\n");
    printf(GREEN "6. " RESET "Modify File\n");
    printf(GREEN "7. " RESET "Upload Photo/Document\n");
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
    char ch;
    int i = 0;
    while ((ch = _getch()) != '\r') {  // Enter key
        if ((ch == '\b' || ch == 127) && i > 0) {
            i--;
            printf("\b \b");
        } else if (i < MAX - 1 && isprint(ch)) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
}

void viewMetadata(const char *filename) {
    struct stat info;
    if (stat(filename, &info) == 0) {
        printf(YELLOW "\n--- File Metadata ---\n" RESET);
        printf(BLUE "Name: " RESET "%s\n", filename);
        printf(BLUE "Size: " RESET "%ld bytes\n", info.st_size);
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

void uploadBinaryFile() {
    char src[MAX], dest[MAX];
    printf(CYAN "\nEnter path of image or document to upload: " RESET);
    fgets(src, MAX, stdin);
    src[strcspn(src, "\n")] = 0;

    printf(YELLOW "Enter destination path/filename: " RESET);
    fgets(dest, MAX, stdin);
    dest[strcspn(dest, "\n")] = 0;

    FILE *source = fopen(src, "rb");
    FILE *target = fopen(dest, "wb");

    if (!source || !target) {
        printf(RED "\n✘ Error opening source or destination file.\n" RESET);
        if (source) fclose(source);
        if (target) fclose(target);
        return;
    }

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytes, target);
    }

    fclose(source);
    fclose(target);

    printf(GREEN "\n✔ Upload successful!\n" RESET);
}
