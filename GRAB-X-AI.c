//Grab-X-AI
// Developers Gajendra Awasthi , Rejina Pujara , Asmita Bist , Bibhu Shrestha
// under supervision of BCA Department NAST College

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

#define TMP_RESPONSE_FILE "response.json"
#define MAX_INPUT 1024
#define MAX_JSON 2048
#define MAX_COMMAND 4096
#define MAX_RESPONSE 4096
#define PASSWORD "cybgaz" // Default password
#define MAX_ATTEMPTS 3
#define SCREEN_WIDTH 80  // Standard terminal width

// Function prototypes
bool authenticate();
void showLoginBox();
void clearScreen();
void showBanner();
void showMenu();
void showHelp();
void showAbout();
void startChat();
void showChatHistory();
char* escapeJson(const char* input);
void sendToGemini(const char* userInput, char* response, size_t responseSize);
void saveChat(const char* user, const char* response);
void clearJsonFile();
void initJsonFile();
void centerText(const char* text);
void printWrappedText(const char* text, int width);

// Embed API key here
const char *API_KEY = "Enter Your Personal Gemini 1.5flash version Api Key";

int main() {
    if (!authenticate()) {
        printf("\nToo many failed attempts. Exiting...\n");
        return 1;
    }

    char choice;
    if (strlen(API_KEY) < 10) {
        fprintf(stderr, "Error: Invalid or missing API key.\n");
        return 1;
    }

    do {
        clearScreen();
        showBanner();
        showMenu();
        printf(" Enter Value: ");
        scanf(" %c", &choice);
        getchar(); // Clear newline

        switch (choice) {
        case 'h':
            showHelp();
            break;
        case 'a':
            showAbout();
            break;
        case 'c':
            startChat();
            break;
        case 'o':
            showChatHistory();
            break;
        case 'x':
            printf("\nExiting GRAB-X-AI. Goodbye!\n");
            break;
        default:
            printf("Invalid input. Try again.\n");
            break;
        }

        if (choice != 'x') {
            printf("\nPress Enter to continue...");
            getchar();
        }

    } while (choice != 'x');

    return 0;
}

bool authenticate() {
    int attempts = 0;
    char input[50];

    while (attempts < MAX_ATTEMPTS) {
        clearScreen();
        showLoginBox();
        printf(" Enter Password: ");

        // Hide password input
        system("stty -echo");
        fgets(input, sizeof(input), stdin);
        system("stty echo");

        input[strcspn(input, "\n")] = '\0'; // Remove newline

        if (strcmp(input, PASSWORD) == 0) {
            return true;
        } else {
            attempts++;
            printf("\n\n\x1b[31mWrong password! %d attempts remaining.\x1b[0m\n", MAX_ATTEMPTS - attempts);
            sleep(1);
        }
    }
    return false;
}

void centerText(const char* text) {
    int pad = (SCREEN_WIDTH - strlen(text)) / 2;
    if (pad < 0) pad = 0;
    printf("%*s%s%*s\n", pad, "", text, pad, "");
}

void showLoginBox() {
    printf("\x1b[34m");
    printf("  _____________________________________________________________  \n");
    printf(" /                                                             \\ \n");
    printf("|                                                               |\n");
    centerText("\033[1m   \x1b[33mWELCOME TO GRAB-X-AI SYSTEM\x1b[34m  \033[0m");
    printf("|                                                               |\n");
    centerText("\033[1m     \x1b[36mAuthentication Required\x1b[34m       \033[0m");
    printf("|                                                               |\n");
    printf(" \\_____________________________________________________________/ \n");
    printf("\x1b[0m");
    printf("\n");
}

void printWrappedText(const char* text, int width) {
    int current_pos = 0;
    const char *p = text;
    const char *last_space = NULL;
    int last_space_pos = 0;
    int word_length = 0;
    const char *word_start = p;
    
    while (*p) {
        // Check for word boundaries
        if (*p == ' ' || *p == '\t' || *p == '\n') {
            last_space = p;
            last_space_pos = current_pos;
            word_length = 0;
        } else {
            word_length++;
        }
        
        // Handle words longer than the width
        if (word_length > width) {
            // Print the part that fits
            while (word_start < p && current_pos < width) {
                putchar(*word_start);
                word_start++;
                current_pos++;
            }
            printf("\n");
            current_pos = 0;
            continue;
        }
        
        if (current_pos >= width) {
            if (last_space) {
                // Print up to the last space
                while (text < last_space) {
                    putchar(*text);
                    text++;
                }
                printf("\n");
                text++; // Skip the space
                current_pos = 0;
                p = text;
                last_space = NULL;
                word_start = p;
                continue;
            } else {
                // No space found, force break
                printf("\n");
                current_pos = 0;
            }
        }
        
        if (*p == '\n') {
            printf("\n");
            current_pos = 0;
            p++;
            text = p;
            word_start = p;
            continue;
        }
        
        putchar(*p);
        current_pos++;
        p++;
    }
    printf("\n");
}

void showChatHistory() {
    clearScreen();
    printf("\x1b[36m");
    printf("==============================================* CHAT HISTORY *===============================================\n");
    printf("\x1b[0m");

    FILE *file = fopen("chat.txt", "r");
    if (file) {
        char line[MAX_INPUT];
        while (fgets(line, sizeof(line), file)) {
            if (line[0] == '[') {
                printf("\x1b[33m%s\x1b[0m", line); // Highlight timestamps in yellow
            } else {
                printWrappedText(line, SCREEN_WIDTH);
            }
        }
        fclose(file);
    } else {
        printf("\x1b[31mNo chat history found!\x1b[0m\n");
    }

    printf("\x1b[36m");
    printf("============================================================================================================\n");
    printf("\x1b[0m");
    printf("\nPress Enter to return to menu...");
    getchar();
}

void clearScreen() {
    printf("\033[H\033[J");
}

void showBanner() {
    printf("\x1b[31m");
    printf(" _____                                                                   _____ \n");
    printf("( ___ )                                                                 ( ___ )\n");
    printf(" |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   | \n");
    printf(" |   |   ____ ____      _    ____           __  __             _    ___  |   | \n");
    printf(" |   |  / ___|  _ \\    / \\  | __ )          \\ \\/ /            / \\  |_ _| |   | \n");
    printf(" |   | | |  _| |_) |  / _ \\ |  _ \\   _____   \\  /   _____    / _ \\  | |  |   | \n");
    printf(" |   | | |_| |  _ <  / ___ \\| |_) | |_____|  /  \\  |_____|  / ___ \\ | |  |   | \n");
    printf(" |   |  \\____|_| \\_\\/_/   \\_\\____/          /_/\\_\\         /_/   \\_\\___| |   | \n");
    printf(" |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___| \n");
    printf("(_____)                                                                 (_____) \n");

    printf("\x1b[0m");
}

void showMenu() {
    printf("\n\n");
    printf(" 1. Press \"h\" for help menu!\n");
    printf(" 2. Press \"a\" for learn about developers!\n");
    printf(" 3. Press \"c\" for chat with AI!\n");
    printf(" 4. Press \"o\" to view old chats\n");
    printf(" 5. Press \"x\" for exit program!\n\n");
}

void showHelp() {
    printf("\x1b[36m");
    printf("===============================================* HELP MENU *===========================================================\n");
    printf("\x1b[0m");
    printf("\x1b[33m");
    printf("\033[1m Welcome to the GRAB-X-AI Help Section\033[0m\n");
    printf("\x1b[0m");
    printf("\x1b[36m");
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    printf("\x1b[0m");
    printf("\x1b[32m");
    printf("\033[1m Project: GRAB-X-AI \033[0m\n");
    printf(" Team: Gajendra Awasthi , Rejina Pujara , Asmita Bist , Bibhu Srestha\n\n");
    printf(" Commands: \n");
    printf(" [h] Help    - Show this help menu\n");
    printf(" [a] About   - Learn about the developers\n");
    printf(" [c] Chat    - Start chatting with GRAB-X-AI\n");
    printf(" [o] Old     - View your previous chat history\n");
    printf(" [x] Exit    - Exit the program\n");
    printf("\n - All chats are automatically saved to 'chat.txt' and 'chat_history.json'.\n");
    printf("\x1b[0m");
    printf("\x1b[36m");
    printf("=================================================* GRAB - X - AI *=====================================================\n");
    printf("\x1b[0m");
}

void showAbout() {
    printf("\x1b[36m");
    printf("===============================================* ABOUT DEVELOPERS *===================================================\n");
    printf("\x1b[0m");
    printf("\x1b[32m");
    printf(" This software was developed under the supervision of our college NATIONAL ACADEMY OF SCIENCE AND TECHNOLOGY BCA Department.\n");
    printf("\n");
    printf(" The Developer Team:\n");
    printf(" -> Gajendra Awasthi\n");
    printf(" -> Rejina Pujara\n");
    printf(" -> Asmita Bist\n");
    printf(" -> Bibhu Srestha\n");
    printf("\x1b[0m");
    printf("\x1b[36m");
    printf("======================================================================================================================\n");
    printf("\x1b[0m");
}

void startChat() {
    char input[MAX_INPUT];
    char response[MAX_RESPONSE];

    initJsonFile(); // Initialize JSON file at start of chat

    printf("\033[36m ===============================================* CHAT SECTION *==================================================\033[0m\n");
    printf(" Enter Value:\n Press \"1\" to chat with AI\n Press \"x\" to return to menu\n\n");
    printf("\033[36m ===============================================* CHAT SECTION *==================================================\033[0m\n");
    char sub;
    scanf(" %c", &sub);
    getchar(); // Clear newline

    if (sub == 'x') return;

    while (1) {
        printf("\nYou: ");
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Error reading input.\n");
            continue;
        }
        input[strcspn(input, "\n")] = 0; // Remove newline

        if (strcmp(input, "x") == 0) {
            printf("Returning to menu...\n");
            break;
        }

        if (strlen(input) == 0) {
            printf("Please enter a non-empty message.\n");
            continue;
        }

        sendToGemini(input, response, sizeof(response));
        printf("GRAB-X-AI: ");
        printWrappedText(response, SCREEN_WIDTH - 12); // Reserve space for "GRAB-X-AI: "

        // Automatically save every chat
        saveChat(input, response);
    }
}

char* escapeJson(const char* input) {
    size_t len = strlen(input);
    char* escaped = malloc(len * 6 + 1); // More space for potential escapes
    if (!escaped) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        switch (input[i]) {
        case '"':
            escaped[j++] = '\\';
            escaped[j++] = '"';
            break;
        case '\\':
            escaped[j++] = '\\';
            escaped[j++] = '\\';
            break;
        case '\n':
            escaped[j++] = '\\';
            escaped[j++] = 'n';
            break;
        case '\r':
            escaped[j++] = '\\';
            escaped[j++] = 'r';
            break;
        case '\t':
            escaped[j++] = '\\';
            escaped[j++] = 't';
            break;
        case '\b':
            escaped[j++] = '\\';
            escaped[j++] = 'b';
            break;
        case '\f':
            escaped[j++] = '\\';
            escaped[j++] = 'f';
            break;
        default:
            if ((unsigned char)input[i] < ' ') {
                // Escape other control characters
                sprintf(escaped + j, "\\u%04x", (unsigned char)input[i]);
                j += 6;
            } else {
                escaped[j++] = input[i];
            }
            break;
        }
    }
    escaped[j] = '\0';
    return escaped;
}

void sendToGemini(const char* userInput, char* response, size_t responseSize) {
    char jsonData[MAX_JSON];
    char command[MAX_COMMAND];

    char* escapedInput = escapeJson(userInput);
    if (!escapedInput) {
        snprintf(response, responseSize, "Error: Failed to escape input.");
        return;
    }

    int written = snprintf(jsonData, sizeof(jsonData),
                           "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}]}",
                           escapedInput
                          );
    free(escapedInput);

    if (written >= sizeof(jsonData)) {
        snprintf(response, responseSize, "Error: Input too long for JSON payload.");
        return;
    }

    written = snprintf(command, sizeof(command),
                       "curl -s -X POST \"https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=%s\" "
                       "-H \"Content-Type: application/json\" "
                       "-d '%s' > %s",
                       API_KEY, jsonData, TMP_RESPONSE_FILE
                      );

    if (written >= sizeof(command)) {
        snprintf(response, responseSize, "Error: Command buffer overflow.");
        return;
    }

    int ret = system(command);
    if (ret != 0) {
        snprintf(response, responseSize, "Error: Failed to execute curl command.");
        return;
    }

    FILE *fp = fopen(TMP_RESPONSE_FILE, "r");
    if (!fp) {
        snprintf(response, responseSize, "Error: Failed to read response file.");
        return;
    }

    char buffer[MAX_RESPONSE];
    size_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[bytesRead] = '\0';
    fclose(fp);

    // Parse the JSON response
    char *textPtr = buffer;
    char *textStart = NULL;
    char *textEnd = NULL;

    // Look for the text in the nested JSON structure
    while ((textPtr = strstr(textPtr, "\"text\"")) != NULL) {
        textPtr += 6; // Move past "\"text\""
        while (*textPtr && *textPtr != ':') textPtr++;
        if (*textPtr == ':') {
            textPtr++;
            while (*textPtr && (*textPtr == ' ' || *textPtr == '\t' || *textPtr == '\n' || *textPtr == '\r')) textPtr++;
            if (*textPtr == '"') {
                textStart = textPtr + 1;
                textEnd = textStart;
                while (*textEnd && *textEnd != '"') {
                    if (*textEnd == '\\' && *(textEnd + 1) == '"') {
                        textEnd += 2; // Skip escaped quotes
                    } else {
                        textEnd++;
                    }
                }
                if (*textEnd == '"') {
                    break;
                }
            }
        }
    }

    if (textStart && textEnd) {
        size_t textLen = textEnd - textStart;
        if (textLen >= responseSize) {
            textLen = responseSize - 1;
        }

        // Unescape the response
        char *out = response;
        char *in = textStart;
        while (in < textEnd && (out - response) < responseSize - 1) {
            if (*in == '\\' && *(in + 1) == 'n') {
                *out++ = '\n';
                in += 2;
            } else if (*in == '\\' && *(in + 1) == '\\') {
                *out++ = '\\';
                in += 2;
            } else if (*in == '\\' && *(in + 1) == '"') {
                *out++ = '"';
                in += 2;
            } else if (*in == '\\' && *(in + 1) == 't') {
                *out++ = '\t';
                in += 2;
            } else {
                *out++ = *in++;
            }
        }
        *out = '\0';
    } else if (strstr(buffer, "\"error\"")) {
        char *errorStart = strstr(buffer, "\"message\":\"");
        if (errorStart) {
            errorStart += 11;
            char *errorEnd = strchr(errorStart, '"');
            if (errorEnd) {
                *errorEnd = '\0';
                snprintf(response, responseSize, "API Error: %s", errorStart);
            } else {
                snprintf(response, responseSize, "API Error: Check response.json for details.");
            }
        } else {
            snprintf(response, responseSize, "API Error: Check response.json for details.");
        }
    } else {
        snprintf(response, responseSize, "Error: Could not parse AI response. Raw response: %s", buffer);
    }
}

void saveChat(const char* user, const char* response) {
    // Get current time with more precise formatting
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    char timestamp[80];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", local);

    // Save to chat.txt with timestamp
    FILE *txtFile = fopen("chat.txt", "a");
    if (txtFile) {
        fprintf(txtFile, "%s\n", timestamp);
        fprintf(txtFile, "You: %s\n", user);
        fprintf(txtFile, "GRAB-X-AI: %s\n\n", response);
        fclose(txtFile);
    } else {
        printf("Error: Failed to save chat to chat.txt.\n");
    }

    // Save to chat_history.json with more complete timestamp
    FILE *jsonFile = fopen("chat_history.json", "a");
    if (jsonFile) {
        char *escapedUser = escapeJson(user);
        char *escapedResponse = escapeJson(response);
        
        if (!escapedUser || !escapedResponse) {
            printf("Error: Failed to escape JSON strings.\n");
            free(escapedUser);
            free(escapedResponse);
            fclose(jsonFile);
            return;
        }

        // More detailed timestamp for JSON
        char jsonTimestamp[80];
        strftime(jsonTimestamp, sizeof(jsonTimestamp), "%Y-%m-%dT%H:%M:%S%z", local);
        
        fprintf(jsonFile, "{\"timestamp\":\"%s\",\"user\":\"%s\",\"response\":\"%s\"}\n",
                jsonTimestamp, escapedUser, escapedResponse);
        
        free(escapedUser);
        free(escapedResponse);
        fclose(jsonFile);
    } else {
        printf("Error: Failed to save chat to JSON file.\n");
    }
}

void initJsonFile() {
    FILE *jsonFile = fopen("chat_history.json", "a");
    if (jsonFile) {
        fclose(jsonFile);
    } else {
        printf("Error: Failed to initialize chat history file.\n");
    }
}

void clearJsonFile() {
    FILE *jsonFile = fopen("chat_history.json", "w");
    if (jsonFile) {
        fclose(jsonFile);
    } else {
        printf("Error: Failed to clear chat history file.\n");
    }
}