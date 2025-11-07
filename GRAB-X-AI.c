//Grab-X-AI
// Developers Gajendra Awasthi , Rejina Pujara , Asmita Bist , Bibhu Shrestha
// under supervision of BCA Department NAST College

// Include necessary standard libraries
#include <stdio.h>       // For input/output operations
#include <stdlib.h>      // For standard library functions
#include <string.h>      // For string manipulation functions
#include <stdbool.h>     // For boolean data type
#include <unistd.h>      // For POSIX operating system API
#include <time.h>        // For time-related functions
#include <ctype.h>       // For character handling functions

// Define constants
#define TMP_RESPONSE_FILE "response.json"  // Temporary file for API responses
#define MAX_INPUT 1024                     // Maximum input length
#define MAX_JSON 2048                      // Maximum JSON payload size
#define MAX_COMMAND 4096                   // Maximum command length
#define MAX_RESPONSE 4096                  // Maximum response length
#define PASSWORD "cybgaz"                  // Default password
#define MAX_ATTEMPTS 3                     // Maximum login attempts
#define SCREEN_WIDTH 80                    // Standard terminal width

// Function prototypes
bool authenticate();                        // Function to authenticate user
void showLoginBox();                       // Function to display login box
void clearScreen();                        // Function to clear terminal screen
void showBanner();                         // Function to display program banner
void showMenu();                           // Function to display main menu
void showHelp();                           // Function to display help menu
void showAbout();                          // Function to display about information
void startChat();                          // Function to start chat with AI
void showChatHistory();                    // Function to show chat history
char* escapeJson(const char* input);       // Function to escape JSON strings
void sendToGemini(const char* userInput, char* response, size_t responseSize); // Function to send query to Gemini API
void saveChat(const char* user, const char* response); // Function to save chat history
void clearJsonFile();                      // Function to clear JSON file
void initJsonFile();                       // Function to initialize JSON file
void centerText(const char* text);         // Function to center text on screen
void printWrappedText(const char* text, int width); // Function to print wrapped text

// Embed API key here
const char *API_KEY = "Enter Your Personal Gemini 1.5flash version Api Key";

// Main function - program entry point
int main() {
    // Authenticate user
    if (!authenticate()) {
        printf("\nToo many failed attempts. Exiting...\n");
        return 1;  // Exit with error code if authentication fails
    }

    char choice;  // Variable to store user's menu choice
    
    // Check if API key is valid
    if (strlen(API_KEY) < 10) {
        fprintf(stderr, "Error: Invalid or missing API key.\n");
        return 1;  // Exit if API key is invalid
    }

    // Main program loop
    do {
        clearScreen();  // Clear the screen
        showBanner();   // Display program banner
        showMenu();     // Display main menu
        printf(" Enter Value: ");
        scanf(" %c", &choice);  // Get user input
        getchar(); // Clear newline from input buffer

        // Process user choice
        switch (choice) {
        case 'h':
            showHelp();  // Show help menu
            break;
        case 'a':
            showAbout();  // Show about information
            break;
        case 'c':
            startChat();  // Start chat with AI
            break;
        case 'o':
            showChatHistory();  // Show chat history
            break;
        case 'x':
            printf("\nExiting GRAB-X-AI. Goodbye!\n");  // Exit program
            break;
        default:
            printf("Invalid input. Try again.\n");  // Handle invalid input
            break;
        }

        // Pause before continuing (except when exiting)
        if (choice != 'x') {
            printf("\nPress Enter to continue...");
            getchar();
        }

    } while (choice != 'x');  // Continue until user chooses to exit

    return 0;  // Exit program successfully
}

// Function to authenticate user with password
bool authenticate() {
    int attempts = 0;         // Track login attempts
    char input[50];           // Buffer for password input

    // Allow up to MAX_ATTEMPTS attempts
    while (attempts < MAX_ATTEMPTS) {
        clearScreen();        // Clear screen
        showLoginBox();       // Show login box
        printf(" Enter Password: ");

        // Hide password input
        system("stty -echo"); // Disable echo
        fgets(input, sizeof(input), stdin);  // Get password input
        system("stty echo");  // Re-enable echo

        input[strcspn(input, "\n")] = '\0'; // Remove newline character

        // Check if password is correct
        if (strcmp(input, PASSWORD) == 0) {
            return true;  // Return true if password matches
        } else {
            attempts++;    // Increment attempt counter
            printf("\n\n\x1b[31mWrong password! %d attempts remaining.\x1b[0m\n", MAX_ATTEMPTS - attempts);
            sleep(1);      // Pause before next attempt
        }
    }
    return false;  // Return false if max attempts reached
}

// Function to center text on screen
void centerText(const char* text) {
    int pad = (SCREEN_WIDTH - strlen(text)) / 2;  // Calculate padding
    if (pad < 0) pad = 0;  // Ensure padding isn't negative
    printf("%*s%s%*s\n", pad, "", text, pad, "");  // Print centered text
}

// Function to display login box
void showLoginBox() {
    printf("\x1b[34m");  // Set blue color
    printf("  _____________________________________________________________  \n");
    printf(" /                                                             \\ \n");
    printf("|                                                               |\n");
    centerText("\033[1m   \x1b[33mWELCOME TO GRAB-X-AI SYSTEM\x1b[34m  \033[0m");  // Centered title
    printf("|                                                               |\n");
    centerText("\033[1m     \x1b[36mAuthentication Required\x1b[34m       \033[0m");  // Centered subtitle
    printf("|                                                               |\n");
    printf(" \\_____________________________________________________________/ \n");
    printf("\x1b[0m");  // Reset color
    printf("\n");
}

// Function to print text with word wrapping
void printWrappedText(const char* text, int width) {
    int current_pos = 0;          // Track current position in line
    const char *p = text;         // Pointer to current character
    const char *last_space = NULL; // Pointer to last space character
    int last_space_pos = 0;       // Position of last space
    int word_length = 0;          // Length of current word
    const char *word_start = p;   // Start of current word
    
    // Process each character
    while (*p) {
        // Check for word boundaries
        if (*p == ' ' || *p == '\t' || *p == '\n') {
            last_space = p;        // Remember last space position
            last_space_pos = current_pos;
            word_length = 0;      // Reset word length
        } else {
            word_length++;         // Increment word length
        }
        
        // Handle words longer than the width
        if (word_length > width) {
            // Print the part that fits
            while (word_start < p && current_pos < width) {
                putchar(*word_start);  // Print character
                word_start++;
                current_pos++;
            }
            printf("\n");          // New line
            current_pos = 0;       // Reset position
            continue;
        }
        
        // Check if we've reached line width
        if (current_pos >= width) {
            if (last_space) {
                // Print up to the last space
                while (text < last_space) {
                    putchar(*text);  // Print character
                    text++;
                }
                printf("\n");      // New line
                text++;            // Skip the space
                current_pos = 0;   // Reset position
                p = text;          // Reset pointer
                last_space = NULL; // Reset space pointer
                word_start = p;    // Reset word start
                continue;
            } else {
                // No space found, force break
                printf("\n");      // New line
                current_pos = 0;   // Reset position
            }
        }
        
        // Handle newlines
        if (*p == '\n') {
            printf("\n");          // New line
            current_pos = 0;       // Reset position
            p++;                   // Move to next character
            text = p;              // Reset text pointer
            word_start = p;        // Reset word start
            continue;
        }
        
        putchar(*p);  // Print character
        current_pos++; // Increment position
        p++;          // Move to next character
    }
    printf("\n");      // Final newline
}

// Function to display chat history
void showChatHistory() {
    clearScreen();  // Clear screen
    printf("\x1b[36m");  // Set cyan color
    printf("==============================================* CHAT HISTORY *===============================================\n");
    printf("\x1b[0m");   // Reset color

    // Open chat history file
    FILE *file = fopen("chat.txt", "r");
    if (file) {
        char line[MAX_INPUT];  // Buffer for each line
        // Read and display each line
        while (fgets(line, sizeof(line), file)) {
            if (line[0] == '[') {
                printf("\x1b[33m%s\x1b[0m", line); // Highlight timestamps in yellow
            } else {
                printWrappedText(line, SCREEN_WIDTH);  // Print wrapped text
            }
        }
        fclose(file);  // Close file
    } else {
        printf("\x1b[31mNo chat history found!\x1b[0m\n");  // Error if no file
    }

    printf("\x1b[36m");  // Set cyan color
    printf("============================================================================================================\n");
    printf("\x1b[0m");   // Reset color
    printf("\nPress Enter to return to menu...");
    getchar();  // Wait for user input
}

// Function to clear terminal screen
void clearScreen() {
    printf("\033[H\033[J");  // ANSI escape codes to clear screen
}

// Function to display program banner
void showBanner() {
    printf("\x1b[31m");  // Set red color
    // ASCII art banner
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

    printf("\x1b[0m");  // Reset color
}

// Function to display main menu
void showMenu() {
    printf("\n\n");
    printf(" 1. Press \"h\" for help menu!\n");
    printf(" 2. Press \"a\" for learn about developers!\n");
    printf(" 3. Press \"c\" for chat with AI!\n");
    printf(" 4. Press \"o\" to view old chats\n");
    printf(" 5. Press \"x\" for exit program!\n\n");
}

// Function to display help information
void showHelp() {
    printf("\x1b[36m");  // Set cyan color
    printf("===============================================* HELP MENU *===========================================================\n");
    printf("\x1b[0m");   // Reset color
    printf("\x1b[33m");  // Set yellow color
    printf("\033[1m Welcome to the GRAB-X-AI Help Section\033[0m\n");
    printf("\x1b[0m");   // Reset color
    printf("\x1b[36m");  // Set cyan color
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    printf("\x1b[0m");   // Reset color
    printf("\x1b[32m");  // Set green color
    printf("\033[1m Project: GRAB-X-AI \033[0m\n");
    printf(" Team: Gajendra Awasthi , Rejina Pujara , Asmita Bist , Bibhu Srestha\n\n");
    printf(" Commands: \n");
    printf(" [h] Help    - Show this help menu\n");
    printf(" [a] About   - Learn about the developers\n");
    printf(" [c] Chat    - Start chatting with GRAB-X-AI\n");
    printf(" [o] Old     - View your previous chat history\n");
    printf(" [x] Exit    - Exit the program\n");
    printf("\n - All chats are automatically saved to 'chat.txt' and 'chat_history.json'.\n");
    printf("\x1b[0m");   // Reset color
    printf("\x1b[36m");  // Set cyan color
    printf("=================================================* GRAB - X - AI *=====================================================\n");
    printf("\x1b[0m");   // Reset color
}

// Function to display about information
void showAbout() {
    printf("\x1b[36m");  // Set cyan color
    printf("===============================================* ABOUT DEVELOPERS *===================================================\n");
    printf("\x1b[0m");   // Reset color
    printf("\x1b[32m");  // Set green color
    printf(" This software was developed under the supervision of our college NATIONAL ACADEMY OF SCIENCE AND TECHNOLOGY BCA Department.\n");
    printf("\n");
    printf(" The Developer Team:\n");
    printf(" -> Gajendra Awasthi\n");
    printf(" -> Rejina Pujara\n");
    printf(" -> Asmita Bist\n");
    printf(" -> Bibhu Srestha\n");
    printf("\x1b[0m");   // Reset color
    printf("\x1b[36m");  // Set cyan color
    printf("======================================================================================================================\n");
    printf("\x1b[0m");   // Reset color
}

// Function to start chat with AI
void startChat() {
    char input[MAX_INPUT];      // Buffer for user input
    char response[MAX_RESPONSE]; // Buffer for AI response

    initJsonFile(); // Initialize JSON file for chat history

    // Display chat section header
    printf("\033[36m ===============================================* CHAT SECTION *==================================================\033[0m\n");
    printf(" Enter Value:\n Press \"1\" to chat with AI\n Press \"x\" to return to menu\n\n");
    printf("\033[36m ===============================================* CHAT SECTION *==================================================\033[0m\n");
    
    char sub; // Variable for sub-menu choice
    printf("Choice: ");
    scanf(" %c", &sub); // Get user choice
    getchar(); // Clear newline from input buffer

    if (sub == 'x') return; // Return to main menu if 'x' pressed

    printf("\nChat started! Type 'x' to exit chat.\n");

    // Main chat loop
    while (1) {
        printf("\nYou: "); // Prompt for user input
        fflush(stdout); // Ensure prompt is displayed immediately
        
        // Read user input
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Error reading input.\n");
            continue; // Skip to next iteration on error
        }
        
        input[strcspn(input, "\n")] = 0; // Remove newline character

        // Check for exit command
        if (strcmp(input, "x") == 0) {
            printf("Returning to menu...\n");
            break; // Exit chat loop
        }

        // Check for empty input
        if (strlen(input) == 0) {
            printf("GRAB-X-AI: Please enter a non-empty message.\n");
            continue; // Skip to next iteration
        }

        // Send input to Gemini AI and get response
        sendToGemini(input, response, sizeof(response));
        printf("GRAB-X-AI: ");
        printWrappedText(response, SCREEN_WIDTH - 12); // Display wrapped response

        // Automatically save every chat to history
        saveChat(input, response);
    }
}
// Function to escape special characters in JSON strings
char* escapeJson(const char* input) {
    size_t len = strlen(input);  // Get input length
    // Allocate buffer with extra space for escape sequences
    char* escaped = malloc(len * 6 + 1); 
    if (!escaped) return NULL;  // Return NULL if allocation fails

    size_t j = 0;  // Index for escaped string
    // Process each character
    for (size_t i = 0; i < len; i++) {
        switch (input[i]) {
        case '"':
            escaped[j++] = '\\';  // Escape double quote
            escaped[j++] = '"';
            break;
        case '\\':
            escaped[j++] = '\\';  // Escape backslash
            escaped[j++] = '\\';
            break;
        case '\n':
            escaped[j++] = '\\';  // Escape newline
            escaped[j++] = 'n';
            break;
        case '\r':
            escaped[j++] = '\\';  // Escape carriage return
            escaped[j++] = 'r';
            break;
        case '\t':
            escaped[j++] = '\\';  // Escape tab
            escaped[j++] = 't';
            break;
        case '\b':
            escaped[j++] = '\\';  // Escape backspace
            escaped[j++] = 'b';
            break;
        case '\f':
            escaped[j++] = '\\';  // Escape form feed
            escaped[j++] = 'f';
            break;
        default:
            // Escape control characters
            if ((unsigned char)input[i] < ' ') {
                sprintf(escaped + j, "\\u%04x", (unsigned char)input[i]);
                j += 6;
            } else {
                escaped[j++] = input[i];  // Copy regular characters
            }
            break;
        }
    }
    escaped[j] = '\0';  // Null-terminate escaped string
    return escaped;
}

// Function to send user input to Gemini API and get response
void sendToGemini(const char* userInput, char* response, size_t responseSize) {
    char jsonData[MAX_JSON];  // Buffer for JSON payload
    char command[MAX_COMMAND]; // Buffer for curl command

    // Escape user input for JSON
    char* escapedInput = escapeJson(userInput);
    if (!escapedInput) {
        snprintf(response, responseSize, "Error: Failed to escape input.");
        return;
    }

    // Create JSON payload
    int written = snprintf(jsonData, sizeof(jsonData),
                       "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}]}",
                       escapedInput
                      );
    free(escapedInput);  // Free escaped input buffer

    // Check for buffer overflow
    if (written >= sizeof(jsonData)) {
        snprintf(response, responseSize, "Error: Input too long for JSON payload.");
        return;
    }

    // Create curl command to call Gemini API
    written = snprintf(command, sizeof(command),
                       "curl -s -X POST \"https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=%s\" "
                       "-H \"Content-Type: application/json\" "
                       "-d '%s' > %s",
                       API_KEY, jsonData, TMP_RESPONSE_FILE
                      );

    // Check for command buffer overflow
    if (written >= sizeof(command)) {
        snprintf(response, responseSize, "Error: Command buffer overflow.");
        return;
    }

    // Execute curl command
    int ret = system(command);
    if (ret != 0) {
        snprintf(response, responseSize, "Error: Failed to execute curl command.");
        return;
    }

    // Read response from temporary file
    FILE *fp = fopen(TMP_RESPONSE_FILE, "r");
    if (!fp) {
        snprintf(response, responseSize, "Error: Failed to read response file.");
        return;
    }

    char buffer[MAX_RESPONSE];  // Buffer for response data
    size_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, fp);  // Read response
    buffer[bytesRead] = '\0';  // Null-terminate response
    fclose(fp);  // Close file

    // Parse the JSON response
    char *textPtr = buffer;  // Pointer for parsing
    char *textStart = NULL;  // Start of text content
    char *textEnd = NULL;    // End of text content

    // Look for the text in the nested JSON structure
    while ((textPtr = strstr(textPtr, "\"text\"")) != NULL) {
        textPtr += 6; // Move past "\"text\""
        while (*textPtr && *textPtr != ':') textPtr++;
        if (*textPtr == ':') {
            textPtr++;
            // Skip whitespace
            while (*textPtr && (*textPtr == ' ' || *textPtr == '\t' || *textPtr == '\n' || *textPtr == '\r')) textPtr++;
            if (*textPtr == '"') {
                textStart = textPtr + 1;  // Start of text content
                textEnd = textStart;
                // Find end of text content
                while (*textEnd && *textEnd != '"') {
                    if (*textEnd == '\\' && *(textEnd + 1) == '"') {
                        textEnd += 2; // Skip escaped quotes
                    } else {
                        textEnd++;
                    }
                }
                if (*textEnd == '"') {
                    break;  // Found complete text content
                }
            }
        }
    }

    // Process found text content
    if (textStart && textEnd) {
        size_t textLen = textEnd - textStart;  // Length of text content
        if (textLen >= responseSize) {
            textLen = responseSize - 1;  // Truncate if too long
        }

        // Unescape the response
        char *out = response;  // Pointer to output buffer
        char *in = textStart;  // Pointer to input text
        while (in < textEnd && (out - response) < responseSize - 1) {
            // Handle escape sequences
            if (*in == '\\' && *(in + 1) == 'n') {
                *out++ = '\n';  // Convert \n to newline
                in += 2;
            } else if (*in == '\\' && *(in + 1) == '\\') {
                *out++ = '\\';  // Convert \\ to backslash
                in += 2;
            } else if (*in == '\\' && *(in + 1) == '"') {
                *out++ = '"';   // Convert \" to quote
                in += 2;
            } else if (*in == '\\' && *(in + 1) == 't') {
                *out++ = '\t';  // Convert \t to tab
                in += 2;
            } else {
                *out++ = *in++; // Copy regular characters
            }
        }
        *out = '\0';  // Null-terminate response
    } else if (strstr(buffer, "\"error\"")) {
        // Handle API errors
        char *errorStart = strstr(buffer, "\"message\":\"");
        if (errorStart) {
            errorStart += 11;  // Move to start of message
            char *errorEnd = strchr(errorStart, '"');
            if (errorEnd) {
                *errorEnd = '\0';  // Terminate message
                snprintf(response, responseSize, "API Error: %s", errorStart);
            } else {
                snprintf(response, responseSize, "API Error: Check response.json for details.");
            }
        } else {
            snprintf(response, responseSize, "API Error: Check response.json for details.");
        }
    } else {
        // Handle parsing errors
        snprintf(response, responseSize, "Error: Could not parse AI response. Raw response: %s", buffer);
    }
}

// Function to save chat to history files
void saveChat(const char* user, const char* response) {
    // Get current time with precise formatting
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    char timestamp[80];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", local);

    // Save to chat.txt with timestamp
    FILE *txtFile = fopen("chat.txt", "a");
    if (txtFile) {
        fprintf(txtFile, "%s\n", timestamp);  // Write timestamp
        fprintf(txtFile, "You: %s\n", user);  // Write user input
        fprintf(txtFile, "GRAB-X-AI: %s\n\n", response);  // Write AI response
        fclose(txtFile);
    } else {
        printf("Error: Failed to save chat to chat.txt.\n");
    }

    // Save to chat_history.json with complete timestamp
    FILE *jsonFile = fopen("chat_history.json", "a");
    if (jsonFile) {
        // Escape strings for JSON
        char *escapedUser = escapeJson(user);
        char *escapedResponse = escapeJson(response);
        
        if (!escapedUser || !escapedResponse) {
            printf("Error: Failed to escape JSON strings.\n");
            free(escapedUser);
            free(escapedResponse);
            fclose(jsonFile);
            return;
        }

        // Create detailed timestamp for JSON
        char jsonTimestamp[80];
        strftime(jsonTimestamp, sizeof(jsonTimestamp), "%Y-%m-%dT%H:%M:%S%z", local);
        
        // Write JSON-formatted chat entry
        fprintf(jsonFile, "{\"timestamp\":\"%s\",\"user\":\"%s\",\"response\":\"%s\"}\n",
                jsonTimestamp, escapedUser, escapedResponse);
        
        free(escapedUser);      // Free escaped user input
        free(escapedResponse);  // Free escaped response
        fclose(jsonFile);       // Close file
    } else {
        printf("Error: Failed to save chat to JSON file.\n");
    }
}

// Function to initialize JSON file
void initJsonFile() {
    FILE *jsonFile = fopen("chat_history.json", "a");  // Open in append mode
    if (jsonFile) {
        fclose(jsonFile);  // Close immediately (just creates file if not exists)
    } else {
        printf("Error: Failed to initialize chat history file.\n");
    }
}

// Function to clear JSON file
void clearJsonFile() {
    FILE *jsonFile = fopen("chat_history.json", "w");  // Open in write mode (truncates)
    if (jsonFile) {
        fclose(jsonFile);  // Close file
    } else {
        printf("Error: Failed to clear chat history file.\n");
    }
}
