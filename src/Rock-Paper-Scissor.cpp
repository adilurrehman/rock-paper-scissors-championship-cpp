#include <iostream>
#include <windows.h>
#include <fstream>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <atomic>

using namespace std;

// Constants
const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 30;
const int TIMER_SECONDS = 30;

// Global variables
atomic<bool> timerExpired(false);
atomic<bool> timerRunning(false);
atomic<int> remainingTime(TIMER_SECONDS);
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE timerThreadHandle = NULL;
string currentUsername = "";
string player2Name = "";

// Function prototypes
void gotoxy(int x, int y);
void centerText(const string& text, int y);
void drawSimpleBox(int x1, int y1, int x2, int y2);
void printHeader();
void printLoginMenu();
void login();
void registerUser();
void showInstructions();
void showMainMenu();
void singlePlayerGame();
void twoPlayerGame();
void gameProcessing(char player1Throw, char player2Throw, int& player1Score, int& player2Score, bool& isDraw, string& resultMsg);
void displayGameScreen(int player1Score, int player2Score, int rounds, int maxRounds, bool isTwoPlayer, string p1Name = "", string p2Name = "");
void displayThrow(char throwType, int playerNum, int x, int y, string playerName = "");
char getPlayerThrow(int playerNum, bool withTimer = true, string playerName = "");
char getAIThrow();
DWORD WINAPI timerThreadFunction(LPVOID lpParam);
void startTimer();
void stopTimer();
void resetTimer();
string throwToString(char throwType);
string getResultMessage(char player1, char player2, int winner);
void showEndGame(int player1Score, int player2Score, bool isTwoPlayer, string p1Name = "", string p2Name = "");
void clearScreenArea(int x1, int y1, int x2, int y2);
int getConsoleWidth();
void updateTimerDisplay(int timeLeft, int width);
void getPlayer2Name();

// Utility functions
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = static_cast<SHORT>(x);
    coord.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int getConsoleWidth() {
    GetConsoleScreenBufferInfo(console, &csbi);
    return static_cast<int>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
}

void centerText(const string& text, int y) {
    int width = getConsoleWidth();
    size_t textLength = text.length();
    int x = static_cast<int>((width - static_cast<int>(textLength)) / 2);
    if (x < 0) x = 0;
    gotoxy(x, y);
    cout << text;
}

void drawSimpleBox(int x1, int y1, int x2, int y2) {
    // Draw top border
    gotoxy(x1, y1);
    cout << "+";
    for (int i = x1 + 1; i < x2; i++) {
        cout << "-";
    }
    cout << "+";

    // Draw sides
    for (int i = y1 + 1; i < y2; i++) {
        gotoxy(x1, i); cout << "|";
        gotoxy(x2, i); cout << "|";
    }

    // Draw bottom border
    gotoxy(x1, y2);
    cout << "+";
    for (int i = x1 + 1; i < x2; i++) {
        cout << "-";
    }
    cout << "+";
}

void clearScreenArea(int x1, int y1, int x2, int y2) {
    for (int y = y1; y <= y2; y++) {
        gotoxy(x1, y);
        for (int x = x1; x <= x2; x++) {
            cout << " ";
        }
    }
}

void updateTimerDisplay(int timeLeft, int width) {
    if (timeLeft <= 5) {
        SetConsoleTextAttribute(console, 12); // Red color for low time
    }
    else if (timeLeft <= 10) {
        SetConsoleTextAttribute(console, 14); // Yellow color for medium time
    }
    else {
        SetConsoleTextAttribute(console, 10); // Green color for normal time
    }

    gotoxy(width - 25, 2);
    cout << "Time: " << setw(2) << setfill('0') << timeLeft << "s ";
    SetConsoleTextAttribute(console, 7); // Reset to white
}

void getPlayer2Name() {
    system("cls");
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 10);
    centerText("|           ENTER PLAYER 2 NAME               |", 11);
    centerText("+---------------------------------------------+", 12);
    SetConsoleTextAttribute(console, 7);

    centerText("Player 1: " + currentUsername, 15);
    centerText("Enter Player 2 Name: ", 17);

    int width = getConsoleWidth();
    gotoxy(width / 2 - 10, 18);
    getline(cin, player2Name);

    // If no name entered, use default
    if (player2Name.empty()) {
        player2Name = "Player 2";
    }

    centerText("Welcome " + player2Name + "!", 20);
    centerText("Press any key to continue...", 22);
    (void)_getch();
}

// Timer functions
DWORD WINAPI timerThreadFunction(LPVOID lpParam) {
    int width = *((int*)lpParam);
    remainingTime = TIMER_SECONDS;
    timerExpired = false;
    timerRunning = true;

    // Initial display
    updateTimerDisplay(remainingTime, width);

    while (remainingTime > 0 && timerRunning) {
        Sleep(1000); // Wait for 1 second

        if (timerRunning) {
            remainingTime--;
            updateTimerDisplay(remainingTime, width);
        }
    }

    if (remainingTime == 0 && timerRunning) {
        timerExpired = true;
        SetConsoleTextAttribute(console, 12); // Red
        int width = getConsoleWidth();
        gotoxy(width / 2 - 10, 19);
        cout << "TIME'S UP!";
        SetConsoleTextAttribute(console, 7); // Reset
    }

    timerRunning = false;
    return 0;
}

void startTimer(int width) {
    stopTimer(); // Stop any existing timer
    resetTimer();

    int* widthPtr = new int(width);
    timerThreadHandle = CreateThread(NULL, 0, timerThreadFunction, widthPtr, 0, NULL);
}

void stopTimer() {
    timerRunning = false;
    if (timerThreadHandle != NULL) {
        WaitForSingleObject(timerThreadHandle, 100);
        CloseHandle(timerThreadHandle);
        timerThreadHandle = NULL;
    }
}

void resetTimer() {
    stopTimer();
    timerExpired = false;
    remainingTime = TIMER_SECONDS;
}

// Game functions
void printHeader() {
    system("cls");
    SetConsoleTextAttribute(console, 11); // Light cyan

    // Get console width for centering
    int width = getConsoleWidth();

    // Draw top border
    gotoxy(0, 0);
    for (int i = 0; i < width; i++) cout << "=";

    // Title with simple ASCII
    centerText("================================================", 1);
    centerText("      ROCK - PAPER - SCISSORS CHAMPIONSHIP      ", 2);
    centerText("             TWO PLAYER EDITION                 ", 3);
    centerText("================================================", 4);

    // Draw bottom border of header
    gotoxy(0, 5);
    for (int i = 0; i < width; i++) cout << "=";

    SetConsoleTextAttribute(console, 7); // Reset to white
}

void printLoginMenu() {
    printHeader();

    SetConsoleTextAttribute(console, 14); // Yellow
    centerText("+---------------------------------------------+", 8);
    centerText("|           ACCOUNT AUTHENTICATION            |", 9);
    centerText("+---------------------------------------------+", 10);

    SetConsoleTextAttribute(console, 7); // White
}

void login() {
    int attempts = 3;
    while (attempts > 0) {
        printLoginMenu();

        string username, password, id, pass;
        bool found = false;

        centerText("Enter Username: ", 12);
        int width = getConsoleWidth();
        gotoxy(width / 2 - 8, 13);
        getline(cin, username);

        centerText("Enter Password: ", 14);
        gotoxy(width / 2 - 8, 15);

        // Password masking
        char ch;
        password = "";
        while ((ch = static_cast<char>(_getch())) != '\r') {
            if (ch == '\b' && !password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
            else if (ch != '\b') {
                password.push_back(ch);
                cout << "*";
            }
        }
        cout << endl;

        // Check credentials
        ifstream loginFile("login.txt");
        while (loginFile >> id >> pass) {
            if (id == username && pass == password) {
                found = true;
                break;
            }
        }
        loginFile.close();

        if (found) {
            currentUsername = username;
            centerText("Login Successful! Welcome " + username + "!", 17);
            SetConsoleTextAttribute(console, 10); // Green
            centerText("Press any key to continue...", 19);
            SetConsoleTextAttribute(console, 7);
            (void)_getch(); // Ignore return value
            return;
        }
        else {
            attempts--;
            centerText("Invalid username or password! Attempts left: " + to_string(attempts), 17);
            if (attempts > 0) {
                centerText("Press any key to try again...", 19);
                (void)_getch(); // Ignore return value
            }
        }
    }

    centerText("Maximum attempts reached! Please register.", 19);
    centerText("Press any key to register...", 20);
    (void)_getch(); // Ignore return value
    registerUser();
}

void registerUser() {
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 8);
    centerText("|                 REGISTRATION                 |", 9);
    centerText("+---------------------------------------------+", 10);
    SetConsoleTextAttribute(console, 7);

    string username, password;
    bool usernameExists = false;
    int width = getConsoleWidth();

    do {
        usernameExists = false;
        centerText("Choose Username: ", 12);
        gotoxy(width / 2 - 8, 13);
        getline(cin, username);

        ifstream loginFile("login.txt");
        string id, pass;
        while (loginFile >> id >> pass) {
            if (id == username) {
                usernameExists = true;
                centerText("Username already taken! Try another.", 15);
                Sleep(1000);
                clearScreenArea(width / 2 - 20, 12, width / 2 + 20, 16);
                break;
            }
        }
        loginFile.close();
    } while (usernameExists);

    centerText("Choose Password: ", 15);
    gotoxy(width / 2 - 8, 16);

    // Password masking
    char ch;
    password = "";
    while ((ch = static_cast<char>(_getch())) != '\r') {
        if (ch == '\b' && !password.empty()) {
            password.pop_back();
            cout << "\b \b";
        }
        else if (ch != '\b') {
            password.push_back(ch);
            cout << "*";
        }
    }
    cout << endl;

    // Save to file
    ofstream loginFile("login.txt", ios::app);
    loginFile << username << " " << password << endl;
    loginFile.close();

    SetConsoleTextAttribute(console, 10);
    centerText("Registration Successful!", 18);
    centerText("Press any key to login...", 20);
    SetConsoleTextAttribute(console, 7);
    (void)_getch(); // Ignore return value

    login();
}

void showInstructions() {
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 7);
    centerText("|                 INSTRUCTIONS                 |", 8);
    centerText("+---------------------------------------------+", 9);
    SetConsoleTextAttribute(console, 7);

    centerText("================================================", 11);
    centerText("  HOW TO PLAY:                                 ", 12);
    centerText("  1. Choose game mode (Single or Two Player)   ", 13);
    centerText("  2. Each player has 30 seconds to make choice ", 14);
    centerText("  3. Enter 'r' for ROCK, 'p' for PAPER,        ", 15);
    centerText("     's' for SCISSORS                          ", 16);
    centerText("                                                ", 17);
    centerText("  WINNING RULES:                               ", 18);
    centerText("  • ROCK crushes SCISSORS                      ", 19);
    centerText("  • SCISSORS cut PAPER                         ", 20);
    centerText("  • PAPER covers ROCK                          ", 21);
    centerText("                                                ", 22);
    centerText("  Each win = 10 points                         ", 23);
    centerText("  Draw = 5 points each                         ", 24);
    centerText("  Best of 5 rounds                             ", 25);
    centerText("================================================", 26);

    SetConsoleTextAttribute(console, 14);
    centerText("Press any key to continue...", 30);
    SetConsoleTextAttribute(console, 7);
    (void)_getch(); // Ignore return value
}

void showMainMenu() {
    printHeader();

    SetConsoleTextAttribute(console, 14);
    centerText("+---------------------------------------------+", 10);
    centerText("|                MAIN MENU                    |", 11);
    centerText("+---------------------------------------------+", 12);
    SetConsoleTextAttribute(console, 7);

    centerText("================================================", 14);
    centerText("  1. SINGLE PLAYER (vs Computer)               ", 15);
    centerText("  2. TWO PLAYERS                               ", 16);
    centerText("  3. VIEW INSTRUCTIONS                         ", 17);
    centerText("  4. VIEW HIGH SCORES                          ", 18);
    centerText("  5. EXIT                                      ", 19);
    centerText("================================================", 20);

    SetConsoleTextAttribute(console, 10);
    int width = getConsoleWidth();
    gotoxy(width / 2 - 12, 24);
    cout << "Select option (1-5): ";
    SetConsoleTextAttribute(console, 7);

    // Position for input - on the next line
    gotoxy(width / 2 - 1, 25);
}

string throwToString(char throwType) {
    switch (toupper(throwType)) {
    case 'R': return "ROCK";
    case 'P': return "PAPER";
    case 'S': return "SCISSORS";
    default: return "INVALID";
    }
}

void displayThrow(char throwType, int playerNum, int x, int y, string playerName) {
    if (playerName.empty()) {
        playerName = (playerNum == 1) ? currentUsername : (playerNum == 2) ? player2Name : "COMPUTER";
    }

    gotoxy(x, y);
    SetConsoleTextAttribute(console, playerNum == 1 ? 11 : (playerNum == 2 ? 13 : 10));
    cout << playerName << " chose: " << throwToString(throwType);

    // Display ASCII art for the throw
    y += 2;
    gotoxy(x, y);

    switch (toupper(throwType)) {
    case 'R': // ROCK
        cout << "    _____" << endl;
        gotoxy(x, y + 1); cout << "---'   __)" << endl;
        gotoxy(x, y + 2); cout << "      (___)" << endl;
        gotoxy(x, y + 3); cout << "      (___)" << endl;
        gotoxy(x, y + 4); cout << "      (__)" << endl;
        gotoxy(x, y + 5); cout << "---.__(_)" << endl;
        break;

    case 'P': // PAPER
        cout << "     _______" << endl;
        gotoxy(x, y + 1); cout << "---'    ____)____" << endl;
        gotoxy(x, y + 2); cout << "           ______)" << endl;
        gotoxy(x, y + 3); cout << "          _______)" << endl;
        gotoxy(x, y + 4); cout << "         _______)" << endl;
        gotoxy(x, y + 5); cout << "---.__________)" << endl;
        break;

    case 'S': // SCISSORS
        cout << "    _______" << endl;
        gotoxy(x, y + 1); cout << "---'   ____)____" << endl;
        gotoxy(x, y + 2); cout << "          ______)" << endl;
        gotoxy(x, y + 3); cout << "       __________)" << endl;
        gotoxy(x, y + 4); cout << "      (____)" << endl;
        gotoxy(x, y + 5); cout << "---.__(___)" << endl;
        break;
    }

    SetConsoleTextAttribute(console, 7);
}

char getPlayerThrow(int playerNum, bool withTimer, string playerName) {
    if (playerName.empty()) {
        playerName = (playerNum == 1) ? currentUsername : player2Name;
    }

    int width = getConsoleWidth();

    gotoxy(width / 2 - static_cast<int>(playerName.length()) / 2 - 4, 10);
    SetConsoleTextAttribute(console, playerNum == 1 ? 11 : 13);
    cout << playerName << "'S TURN";
    SetConsoleTextAttribute(console, 7);

    drawSimpleBox(width / 2 - 20, 12, width / 2 + 20, 20);

    centerText("Choose your move:", 14);
    centerText("'R' for ROCK", 15);
    centerText("'P' for PAPER", 16);
    centerText("'S' for SCISSORS", 17);

    if (withTimer) {
        resetTimer();
        startTimer(width);
    }

    char choice = '\0';

    while (true) {
        if (withTimer && timerExpired) {
            gotoxy(width / 2 - 10, 19);
            SetConsoleTextAttribute(console, 12);
            cout << "TIME'S UP!";
            SetConsoleTextAttribute(console, 7);

            // Random choice if time expires
            srand(static_cast<unsigned int>(time(nullptr)));
            int randomChoice = rand() % 3;
            switch (randomChoice) {
            case 0: choice = 'r'; break;
            case 1: choice = 'p'; break;
            case 2: choice = 's'; break;
            }
            stopTimer();
            break;
        }

        if (_kbhit()) {
            choice = static_cast<char>(tolower(_getch()));
            if (choice == 'r' || choice == 'p' || choice == 's') {
                stopTimer();
                break;
            }
        }

        // Small delay to prevent CPU overuse
        Sleep(50);
    }

    return choice;
}

char getAIThrow() {
    srand(static_cast<unsigned int>(time(nullptr)));
    int i = rand() % 3;
    switch (i) {
    case 0: return 'r';
    case 1: return 'p';
    default: return 's';
    }
}

string getResultMessage(char player1, char player2, int winner) {
    if (winner == 0) return "It's a DRAW!";

    if (winner == 1) {
        if (player1 == 'r' && player2 == 's') return "ROCK crushes SCISSORS!";
        if (player1 == 'p' && player2 == 'r') return "PAPER covers ROCK!";
        if (player1 == 's' && player2 == 'p') return "SCISSORS cut PAPER!";
    }
    else {
        if (player2 == 'r' && player1 == 's') return "ROCK crushes SCISSORS!";
        if (player2 == 'p' && player1 == 'r') return "PAPER covers ROCK!";
        if (player2 == 's' && player1 == 'p') return "SCISSORS cut PAPER!";
    }

    return "Round result!";
}

void gameProcessing(char player1Throw, char player2Throw, int& player1Score, int& player2Score, bool& isDraw, string& resultMsg) {
    isDraw = false;

    if (player1Throw == player2Throw) {
        isDraw = true;
        player1Score += 5;
        player2Score += 5;
        resultMsg = getResultMessage(player1Throw, player2Throw, 0);
        return;
    }

    // Check who wins
    bool player1Wins = false;

    if ((player1Throw == 'r' && player2Throw == 's') ||
        (player1Throw == 'p' && player2Throw == 'r') ||
        (player1Throw == 's' && player2Throw == 'p')) {
        player1Wins = true;
    }

    if (player1Wins) {
        player1Score += 10;
        resultMsg = getResultMessage(player1Throw, player2Throw, 1);
    }
    else {
        player2Score += 10;
        resultMsg = getResultMessage(player1Throw, player2Throw, 2);
    }
}

void displayGameScreen(int player1Score, int player2Score, int rounds, int maxRounds, bool isTwoPlayer, string p1Name, string p2Name) {
    system("cls");
    printHeader();

    int width = getConsoleWidth();

    // Use provided names or defaults
    if (p1Name.empty()) p1Name = currentUsername;
    if (p2Name.empty()) p2Name = isTwoPlayer ? player2Name : "COMPUTER";

    // Score display with player names
    gotoxy(10, 7);
    SetConsoleTextAttribute(console, 11);
    cout << p1Name << ": " << player1Score << " pts";

    gotoxy(width - 30, 7);
    SetConsoleTextAttribute(console, isTwoPlayer ? 13 : 10);
    cout << p2Name << ": " << player2Score << " pts";

    gotoxy(width / 2 - 10, 7);
    SetConsoleTextAttribute(console, 14);
    cout << "ROUND " << rounds << "/" << maxRounds;

    SetConsoleTextAttribute(console, 7);

    // VS separator
    gotoxy(width / 2 - 15, 12);
    for (int i = 0; i < 30; i++) cout << "-";

    gotoxy(width / 2 - 1, 12);
    SetConsoleTextAttribute(console, 12);
    cout << "VS";
    SetConsoleTextAttribute(console, 7);

    for (int i = 0; i < 15; i++) cout << "-";
}

void singlePlayerGame() {
    int player1Score = 0, player2Score = 0;
    int rounds = 1;
    const int maxRounds = 5;

    while (rounds <= maxRounds) {
        int width = getConsoleWidth();
        displayGameScreen(player1Score, player2Score, rounds, maxRounds, false, currentUsername, "COMPUTER");

        // Player 1 turn
        char player1Throw = getPlayerThrow(1, true, currentUsername);

        // AI turn
        char aiThrow = getAIThrow();

        // Display both throws
        displayThrow(player1Throw, 1, 15, 15, currentUsername);
        displayThrow(aiThrow, 3, width - 40, 15, "COMPUTER");

        // Process result
        bool isDraw;
        string resultMsg;
        gameProcessing(player1Throw, aiThrow, player1Score, player2Score, isDraw, resultMsg);

        // Display result
        gotoxy(width / 2 - static_cast<int>(resultMsg.length()) / 2, 25);
        if (isDraw) {
            SetConsoleTextAttribute(console, 14); // Yellow for draw
        }
        else if (player1Score > player2Score) {
            SetConsoleTextAttribute(console, 10); // Green for win
        }
        else {
            SetConsoleTextAttribute(console, 12); // Red for loss
        }
        cout << resultMsg;
        SetConsoleTextAttribute(console, 7);

        // Update scores
        gotoxy(10, 8);
        SetConsoleTextAttribute(console, 11);
        cout << currentUsername << ": " << player1Score << " pts";

        gotoxy(width - 30, 8);
        SetConsoleTextAttribute(console, 10);
        cout << "COMPUTER: " << player2Score << " pts";
        SetConsoleTextAttribute(console, 7);

        rounds++;

        if (rounds <= maxRounds) {
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 3...";
            Sleep(1000);
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 2...";
            Sleep(1000);
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 1...";
            Sleep(1000);
        }
    }

    stopTimer(); // Ensure timer is stopped
    showEndGame(player1Score, player2Score, false, currentUsername, "COMPUTER");
}

void twoPlayerGame() {
    // Get Player 2 name if not already set
    if (player2Name.empty()) {
        getPlayer2Name();
    }

    int player1Score = 0, player2Score = 0;
    int rounds = 1;
    const int maxRounds = 5;

    while (rounds <= maxRounds) {
        int width = getConsoleWidth();
        displayGameScreen(player1Score, player2Score, rounds, maxRounds, true, currentUsername, player2Name);

        // Player 1 turn
        gotoxy(width / 2 - 20, 27);
        cout << currentUsername << "'s turn (screen will clear for privacy)";
        Sleep(2000);
        char player1Throw = getPlayerThrow(1, true, currentUsername);
        system("cls");
        printHeader();

        // Player 2 turn
        gotoxy(width / 2 - 20, 27);
        cout << player2Name << "'s turn (screen will clear for privacy)";
        Sleep(2000);
        char player2Throw = getPlayerThrow(2, true, player2Name);

        // Display both throws
        displayGameScreen(player1Score, player2Score, rounds, maxRounds, true, currentUsername, player2Name);
        displayThrow(player1Throw, 1, 15, 15, currentUsername);
        displayThrow(player2Throw, 2, width - 40, 15, player2Name);

        // Process result
        bool isDraw;
        string resultMsg;
        gameProcessing(player1Throw, player2Throw, player1Score, player2Score, isDraw, resultMsg);

        // Display result
        gotoxy(width / 2 - static_cast<int>(resultMsg.length()) / 2, 25);
        if (isDraw) {
            SetConsoleTextAttribute(console, 14);
        }
        else if (player1Score > player2Score) {
            SetConsoleTextAttribute(console, 10);
        }
        else {
            SetConsoleTextAttribute(console, 12);
        }
        cout << resultMsg;
        SetConsoleTextAttribute(console, 7);

        // Update scores
        gotoxy(10, 8);
        SetConsoleTextAttribute(console, 11);
        cout << currentUsername << ": " << player1Score << " pts";

        gotoxy(width - 30, 8);
        SetConsoleTextAttribute(console, 13);
        cout << player2Name << ": " << player2Score << " pts";
        SetConsoleTextAttribute(console, 7);

        rounds++;

        if (rounds <= maxRounds) {
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 3...";
            Sleep(1000);
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 2...";
            Sleep(1000);
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 1...";
            Sleep(1000);
        }
    }

    stopTimer(); // Ensure timer is stopped
    showEndGame(player1Score, player2Score, true, currentUsername, player2Name);
}

void showEndGame(int player1Score, int player2Score, bool isTwoPlayer, string p1Name, string p2Name) {
    system("cls");
    printHeader();

    int width = getConsoleWidth();

    gotoxy(width / 2 - 20, 10);
    SetConsoleTextAttribute(console, 14);
    cout << "+==========================================+";
    gotoxy(width / 2 - 20, 11);
    cout << "|             GAME OVER!                   |";
    gotoxy(width / 2 - 20, 12);
    cout << "+==========================================+";

    SetConsoleTextAttribute(console, 7);

    // Use provided names or defaults
    if (p1Name.empty()) p1Name = currentUsername;
    if (p2Name.empty()) p2Name = isTwoPlayer ? player2Name : "COMPUTER";

    // Display final scores with player names
    gotoxy(width / 2 - 20, 14);
    SetConsoleTextAttribute(console, 11);
    cout << p1Name << ": " << player1Score << " points";

    gotoxy(width / 2 - 20, 15);
    SetConsoleTextAttribute(console, isTwoPlayer ? 13 : 10);
    cout << p2Name << ": " << player2Score << " points";

    // Determine winner
    gotoxy(width / 2 - 20, 17);
    SetConsoleTextAttribute(console, 14);

    if (player1Score > player2Score) {
        cout << p1Name << " WINS THE GAME!";
    }
    else if (player2Score > player1Score) {
        cout << p2Name << " WINS THE GAME!";
    }
    else {
        cout << "IT'S A TIE!";
    }

    SetConsoleTextAttribute(console, 7);

    // Save high score for single player
    if (!isTwoPlayer && player1Score > 0) {
        ofstream highscore("highscores.txt", ios::app);
        time_t now = time(nullptr);
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &now); // Use safe version
        highscore << p1Name << " - Score: " << player1Score << " | Date: " << buffer;
        highscore.close();
    }

    // Menu
    gotoxy(width / 2 - 20, 20);
    cout << "1. Play Again";
    gotoxy(width / 2 - 20, 21);
    cout << "2. Main Menu";
    gotoxy(width / 2 - 20, 22);
    cout << "3. Exit";

    gotoxy(width / 2 - 20, 24);
    cout << "Select option: ";

    char choice;
    gotoxy(width / 2 - 5, 24);
    cin >> choice;
    cin.ignore();

    stopTimer(); // Ensure timer is stopped

    switch (choice) {
    case '1':
        if (isTwoPlayer) twoPlayerGame();
        else singlePlayerGame();
        break;
    case '2':
        return;
    case '3':
        stopTimer(); // Clean up timer
        exit(0);
    }
}

int main() {
    // Set console window properties
    system("title ROCK PAPER SCISSORS CHAMPIONSHIP");
    system("mode con cols=120 lines=40");

    // Change console font to one that supports ASCII properly
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 8;
    cfi.dwFontSize.Y = 16;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas"); // Use safe version
    SetCurrentConsoleFontEx(console, FALSE, &cfi);

    // Hide cursor
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(console, &cursorInfo);

    // Login
    login();

    // Main game loop
    while (true) {
        showMainMenu();

        char choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case '1':
            singlePlayerGame();
            break;
        case '2':
            twoPlayerGame();
            break;
        case '3':
            showInstructions();
            break;
        case '4':
        {
            system("cls");
            printHeader();
            SetConsoleTextAttribute(console, 14);
            centerText("HIGH SCORES", 10);
            SetConsoleTextAttribute(console, 7);

            ifstream highscore("highscores.txt");
            if (highscore.is_open()) {
                string line;
                int y = 12;
                while (getline(highscore, line)) {
                    centerText(line, y++);
                }
                highscore.close();
            }
            else {
                centerText("No high scores yet!", 12);
            }

            centerText("Press any key to continue...", 30);
            (void)_getch(); // Ignore return value
        }
        break;
        case '5':
            stopTimer(); // Clean up timer
            exit(0);
        default:
            centerText("Invalid choice! Press any key...", 26);
            (void)_getch(); // Ignore return value
        }
    }

    return 0;
}