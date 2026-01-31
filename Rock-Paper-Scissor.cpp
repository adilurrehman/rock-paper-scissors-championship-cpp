#include <iostream>
#include <windows.h>
#include <fstream>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <atomic>
#include <sstream>

using namespace std;

// Constants
const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 30;
const int TIMER_SECONDS = 30;

// Arrow key codes
const int KEY_UP = 72;
const int KEY_DOWN = 80;
const int KEY_LEFT = 75;
const int KEY_RIGHT = 77;
const int KEY_ENTER = 13;
const int KEY_ESC = 27;

// Global variables
atomic<bool> timerExpired(false);
atomic<bool> timerRunning(false);
atomic<int> remainingTime(TIMER_SECONDS);
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE timerThreadHandle = NULL;
string currentUsername = "";
string player2Name = "";

// Screen dimensions
int screenWidth = 120;
int screenHeight = 40;
int centerX = 60;
int centerY = 20;

// Game statistics
int totalGamesPlayed = 0;
int totalWins = 0;
int totalLosses = 0;
int totalDraws = 0;
int highScore = 0;
int currentStreak = 0;
int bestStreak = 0;

// Function prototypes
void gotoxy(int x, int y);
void centerText(const string& text, int y);
void printBox(int x, int y, int width, int height);
void printHeader();
void printDeveloperHeader();
void printLoginMenu();
void printLoadingScreen();
void login();
void registerUser();
void showInstructions();
void showMainMenu();
void showStatistics();
void singlePlayerGame();
void twoPlayerGame();
void gameProcessing(char player1Throw, char player2Throw, int& player1Score, int& player2Score, bool& isDraw, string& resultMsg);
void displayGameScreen(int player1Score, int player2Score, int rounds, int maxRounds, bool isTwoPlayer, string p1Name = "", string p2Name = "");
void displayThrow(char throwType, int playerNum, int x, int y, string playerName = "");
char getPlayerThrow(int playerNum, bool withTimer = true, string playerName = "");
char getAIThrow();
DWORD WINAPI timerThreadFunction(LPVOID lpParam);
void startTimer(int width);
void stopTimer();
void resetTimer();
string throwToString(char throwType);
string getResultMessage(char player1, char player2, int winner);
void showEndGame(int player1Score, int player2Score, bool isTwoPlayer, string p1Name = "", string p2Name = "");
void clearScreenArea(int x1, int y1, int x2, int y2);
int getConsoleWidth();
void updateTimerDisplay(int timeLeft, int width);
void getPlayer2Name();
void hideConsoleCursor();
void showConsoleCursor();
void setConsoleColor(int color);
void getScreenDimensions();
void setConsoleSize();
void playSound(int frequency, int duration);
int navigateMenu(string options[], int numOptions, string title, int startY);
void saveHighScore();
void loadHighScore();
void saveStatistics();
void loadStatistics();
void printCentered(string text, int y);

// Utility functions
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = static_cast<SHORT>(x);
    coord.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideConsoleCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void showConsoleCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void getScreenDimensions() {
    GetConsoleScreenBufferInfo(console, &csbi);
    screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    centerX = screenWidth / 2;
    centerY = screenHeight / 2;
}

void setConsoleSize() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    Sleep(100);
    getScreenDimensions();
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

void printCentered(string text, int y) {
    int x = centerX - (text.length() / 2);
    if (x < 0) x = 0;
    gotoxy(x, y);
    cout << text;
}

void playSound(int frequency, int duration) {
    Beep(frequency, duration);
}

void printBox(int x, int y, int width, int height) {
    setConsoleColor(11); // Cyan
    gotoxy(x, y);
    cout << char(201);
    for (int i = 0; i < width - 2; i++) cout << char(205);
    cout << char(187);
    
    for (int i = 1; i < height - 1; i++) {
        gotoxy(x, y + i);
        cout << char(186);
        gotoxy(x + width - 1, y + i);
        cout << char(186);
    }
    
    gotoxy(x, y + height - 1);
    cout << char(200);
    for (int i = 0; i < width - 2; i++) cout << char(205);
    cout << char(188);
    setConsoleColor(7); // Reset to white
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
        int w = getConsoleWidth();
        gotoxy(w / 2 - 10, 19);
        cout << "TIME'S UP!";
        playSound(300, 500);
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

void printDeveloperHeader() {
    getScreenDimensions();
    int boxWidth = 56;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14); // Yellow
    printBox(startX, 0, boxWidth, 4);
    setConsoleColor(15); // Bright White
    printCentered("ROCK PAPER SCISSORS CHAMPIONSHIP", 1);
    setConsoleColor(10); // Green
    printCentered("Developed by: Adil ur Rehman | Roll: 24-CS-760", 2);
    setConsoleColor(7);
}

void printLoadingScreen() {
    system("cls");
    getScreenDimensions();
    
    int boxWidth = 60;
    int startX = centerX - boxWidth / 2;
    int startY = centerY - 8;
    
    setConsoleColor(14); // Yellow
    printBox(startX, startY, boxWidth, 10);
    
    setConsoleColor(15); // Bright White
    printCentered("**************************************************", startY + 1);
    setConsoleColor(11); // Cyan
    printCentered("*     ROCK - PAPER - SCISSORS CHAMPIONSHIP       *", startY + 2);
    setConsoleColor(10); // Green
    printCentered("*         Developed by: Adil ur Rehman           *", startY + 3);
    printCentered("*              Roll No: 24-CS-760                *", startY + 4);
    setConsoleColor(15);
    printCentered("**************************************************", startY + 5);
    
    // ASCII Art for Rock Paper Scissors
    setConsoleColor(11);
    printCentered("   ROCK      PAPER     SCISSORS", startY + 7);
    printCentered("   (__)      (___)       \\\\//  ", startY + 8);
    
    setConsoleColor(12); // Red
    printCentered("Game Loading", startY + 11);
    
    // Animated loading bar
    int barWidth = 40;
    int barX = centerX - barWidth / 2;
    gotoxy(barX, startY + 12);
    setConsoleColor(8);
    cout << "[";
    for (int i = 0; i < barWidth - 2; i++) cout << " ";
    cout << "]";
    
    for (int i = 0; i < barWidth - 2; i++) {
        gotoxy(barX + 1 + i, startY + 12);
        setConsoleColor(10);
        cout << char(219);
        playSound(200 + i * 15, 30);
        Sleep(40);
    }
    
    setConsoleColor(10);
    printCentered("LOADING COMPLETE!", startY + 14);
    playSound(800, 200);
    playSound(1000, 200);
    
    setConsoleColor(14);
    printCentered("Press any key to continue...", startY + 16);
    setConsoleColor(7);
    _getch();
}

void printHeader() {
    system("cls");
    printDeveloperHeader();
}

void printLoginMenu() {
    printHeader();

    int boxWidth = 50;
    int startX = centerX - boxWidth / 2;
    int startY = 6;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 6);
    
    setConsoleColor(15);
    printCentered("Welcome To Login Page", startY + 2);
    setConsoleColor(11);
    printCentered("Enter Username and Password", startY + 3);
    setConsoleColor(7);
}

int navigateMenu(string options[], int numOptions, string title, int startY) {
    int selected = 0;
    int key;
    
    int boxWidth = 45;
    int startX = centerX - boxWidth / 2;
    
    while (true) {
        setConsoleColor(14);
        printCentered(title, startY - 2);
        setConsoleColor(7);
        
        printBox(startX - 2, startY - 1, boxWidth + 4, numOptions + 2);
        
        for (int i = 0; i < numOptions; i++) {
            gotoxy(startX, startY + i);
            
            if (i == selected) {
                setConsoleColor(240); // White background, black text (highlighted)
                cout << " > " << options[i] << " < ";
                playSound(400, 50);
            } else {
                setConsoleColor(7); // Normal
                cout << "   " << options[i] << "   ";
            }
        }
        setConsoleColor(7);
        
        setConsoleColor(8);
        printCentered("[UP/DOWN: Navigate | ENTER: Select]", startY + numOptions + 2);
        setConsoleColor(7);
        
        key = _getch();
        
        if (key == 0 || key == 224) {
            key = _getch();
            
            if (key == KEY_UP) {
                selected--;
                if (selected < 0) selected = numOptions - 1;
            }
            else if (key == KEY_DOWN) {
                selected++;
                if (selected >= numOptions) selected = 0;
            }
        }
        else if (key == KEY_ENTER) {
            playSound(600, 100);
            return selected;
        }
        else if (key == KEY_ESC) {
            return numOptions - 1; // Return last option (usually exit)
        }
    }
}

void login() {
    int attempts = 3;
    while (attempts > 0) {
        printLoginMenu();
        showConsoleCursor();

        string username, password, id, pass, score_str;
        bool found = false;

        int inputY = 14;
        int startX = centerX - 20;
        
        setConsoleColor(14);
        gotoxy(startX, inputY);
        cout << "=== LOGIN ===";
        setConsoleColor(7);

        gotoxy(startX, inputY + 2);
        cout << "Enter Username: ";
        cin >> username;

        gotoxy(startX, inputY + 3);
        cout << "Enter Password: ";

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
        while (loginFile >> id >> pass >> score_str) {
            if (id == username && pass == password) {
                found = true;
                highScore = stoi(score_str);
                break;
            }
        }
        loginFile.close();

        if (found) {
            currentUsername = username;
            loadStatistics();
            setConsoleColor(10);
            gotoxy(startX, inputY + 5);
            cout << "Welcome back, " << username << "!";
            gotoxy(startX, inputY + 6);
            cout << "Your High Score: " << highScore;
            playSound(800, 200);
            playSound(1000, 200);
            Sleep(1500);
            hideConsoleCursor();
            return;
        }
        else {
            attempts--;
            setConsoleColor(12);
            gotoxy(startX, inputY + 5);
            cout << "Invalid credentials! Attempts left: " << attempts;
            playSound(300, 500);
            if (attempts > 0) {
                gotoxy(startX, inputY + 7);
                cout << "Press any key to try again...";
                _getch();
            }
        }
    }

    setConsoleColor(12);
    printCentered("Maximum attempts reached! Please register.", centerY);
    playSound(300, 500);
    Sleep(1000);
    registerUser();
    hideConsoleCursor();
}

void registerUser() {
    printHeader();
    showConsoleCursor();

    string username, password, id, pass, score_str;
    bool usernameExists = false;

    int inputY = 14;
    int startX = centerX - 20;

    setConsoleColor(14);
    gotoxy(startX, inputY);
    cout << "=== REGISTER ===";
    setConsoleColor(7);

    do {
        usernameExists = false;
        gotoxy(startX, inputY + 2);
        cout << "Choose Username: ";
        cin >> username;

        ifstream loginFile("login.txt");
        while (loginFile >> id >> pass >> score_str) {
            if (id == username) {
                usernameExists = true;
                setConsoleColor(12);
                gotoxy(startX, inputY + 4);
                cout << "Username already taken! Try another.";
                playSound(300, 300);
                Sleep(1000);
                clearScreenArea(startX, inputY + 2, startX + 40, inputY + 5);
                break;
            }
        }
        loginFile.close();
    } while (usernameExists);

    gotoxy(startX, inputY + 3);
    cout << "Choose Password: ";

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
    loginFile << username << " " << password << " " << 0 << endl;
    loginFile.close();

    currentUsername = username;
    highScore = 0;
    totalGamesPlayed = 0;
    totalWins = 0;
    totalLosses = 0;
    totalDraws = 0;
    currentStreak = 0;
    bestStreak = 0;

    setConsoleColor(10);
    gotoxy(startX, inputY + 5);
    cout << "Registration Successful!";
    playSound(600, 150);
    playSound(800, 150);
    playSound(1000, 200);
    
    gotoxy(startX, inputY + 6);
    cout << "Redirecting to game...";
    Sleep(1500);
    setConsoleColor(7);
    hideConsoleCursor();
}

void saveHighScore() {
    // Read all users
    fstream file;
    string content = "";
    string id, pass, score;
    bool found = false;
    
    file.open("login.txt", ios::in);
    while (file >> id >> pass >> score) {
        if (id == currentUsername) {
            content += id + " " + pass + " " + to_string(highScore) + "\n";
            found = true;
        } else {
            content += id + " " + pass + " " + score + "\n";
        }
    }
    file.close();
    
    file.open("login.txt", ios::out);
    file << content;
    file.close();
}

void loadHighScore() {
    fstream file;
    string id, pass, score;
    
    file.open("login.txt", ios::in);
    while (file >> id >> pass >> score) {
        if (id == currentUsername) {
            highScore = stoi(score);
            break;
        }
    }
    file.close();
}

void saveStatistics() {
    ofstream file("stats_" + currentUsername + ".txt");
    file << totalGamesPlayed << " " << totalWins << " " << totalLosses << " " 
         << totalDraws << " " << bestStreak << endl;
    file.close();
}

void loadStatistics() {
    ifstream file("stats_" + currentUsername + ".txt");
    if (file.is_open()) {
        file >> totalGamesPlayed >> totalWins >> totalLosses >> totalDraws >> bestStreak;
        file.close();
    }
}

void showStatistics() {
    system("cls");
    printDeveloperHeader();
    
    int startY = 6;
    int boxWidth = 55;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 18);
    
    setConsoleColor(15);
    printCentered("=== PLAYER STATISTICS ===", startY + 1);
    
    setConsoleColor(11);
    printCentered("Player: " + currentUsername, startY + 3);
    
    setConsoleColor(7);
    gotoxy(startX + 5, startY + 5);
    cout << "Total Games Played: " << totalGamesPlayed;
    
    setConsoleColor(10);
    gotoxy(startX + 5, startY + 7);
    cout << "Total Wins:         " << totalWins;
    
    setConsoleColor(12);
    gotoxy(startX + 5, startY + 8);
    cout << "Total Losses:       " << totalLosses;
    
    setConsoleColor(14);
    gotoxy(startX + 5, startY + 9);
    cout << "Total Draws:        " << totalDraws;
    
    setConsoleColor(11);
    gotoxy(startX + 5, startY + 11);
    cout << "Best Win Streak:    " << bestStreak;
    
    setConsoleColor(10);
    gotoxy(startX + 5, startY + 12);
    cout << "Current Streak:     " << currentStreak;
    
    setConsoleColor(14);
    gotoxy(startX + 5, startY + 14);
    cout << "High Score:         " << highScore;
    
    // Win rate calculation
    if (totalGamesPlayed > 0) {
        float winRate = (float)totalWins / totalGamesPlayed * 100;
        setConsoleColor(15);
        gotoxy(startX + 5, startY + 15);
        cout << "Win Rate:           " << fixed << setprecision(1) << winRate << "%";
    }
    
    setConsoleColor(8);
    printCentered("Press any key to continue...", startY + 17);
    setConsoleColor(7);
    _getch();
}

void showInstructions() {
    system("cls");
    printDeveloperHeader();
    
    int startY = 5;
    int boxWidth = 60;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, startY, boxWidth, 20);
    
    setConsoleColor(15);
    printCentered("=== HOW TO PLAY ===", startY + 1);
    
    setConsoleColor(7);
    printCentered("1. Choose game mode (Single or Two Player)", startY + 3);
    printCentered("2. Each player has 30 seconds to make choice", startY + 4);
    printCentered("3. Use Arrow Keys to navigate menus", startY + 5);
    printCentered("4. Press ENTER to select your choice", startY + 6);
    
    setConsoleColor(14);
    printCentered("=== WINNING RULES ===", startY + 8);
    
    setConsoleColor(10);
    printCentered("ROCK crushes SCISSORS", startY + 10);
    setConsoleColor(12);
    printCentered("SCISSORS cut PAPER", startY + 11);
    setConsoleColor(11);
    printCentered("PAPER covers ROCK", startY + 12);
    
    setConsoleColor(14);
    printCentered("=== SCORING ===", startY + 14);
    
    setConsoleColor(7);
    printCentered("Win = 10 points | Draw = 5 points each", startY + 16);
    printCentered("Best of 5 rounds per match", startY + 17);
    
    setConsoleColor(8);
    printCentered("Press any key to continue...", startY + 19);
    setConsoleColor(7);
    playSound(500, 100);
    _getch();
}

void getPlayer2Name() {
    system("cls");
    printDeveloperHeader();
    showConsoleCursor();

    int boxWidth = 50;
    int startX = centerX - boxWidth / 2;
    
    setConsoleColor(14);
    printBox(startX, 8, boxWidth, 12);
    
    setConsoleColor(15);
    printCentered("ENTER PLAYER 2 NAME", 10);
    setConsoleColor(7);

    printCentered("Player 1: " + currentUsername, 13);
    
    gotoxy(centerX - 15, 16);
    cout << "Enter Player 2 Name: ";
    
    cin.ignore();
    getline(cin, player2Name);

    // If no name entered, use default
    if (player2Name.empty()) {
        player2Name = "Player 2";
    }

    setConsoleColor(10);
    printCentered("Welcome " + player2Name + "!", 18);
    playSound(600, 150);
    Sleep(1000);
    hideConsoleCursor();
}

void showMainMenu() {
    system("cls");
    printDeveloperHeader();
    
    // Display current stats
    setConsoleColor(14);
    gotoxy(5, 5);
    cout << "Player: " << currentUsername;
    gotoxy(5, 6);
    setConsoleColor(10);
    cout << "High Score: " << highScore;
    gotoxy(5, 7);
    setConsoleColor(11);
    cout << "Win Streak: " << currentStreak;
    
    setConsoleColor(7);
    
    string menuOptions[] = {
        "SINGLE PLAYER (vs Computer)",
        "TWO PLAYERS (Local)        ",
        "VIEW INSTRUCTIONS          ",
        "VIEW STATISTICS            ",
        "VIEW HIGH SCORES           ",
        "EXIT GAME                  "
    };
    
    int choice = navigateMenu(menuOptions, 6, "=== MAIN MENU ===", 12);
    
    switch (choice) {
        case 0:
            singlePlayerGame();
            break;
        case 1:
            twoPlayerGame();
            break;
        case 2:
            showInstructions();
            break;
        case 3:
            showStatistics();
            break;
        case 4:
            {
                system("cls");
                printDeveloperHeader();
                
                int boxWidth = 55;
                int startX = centerX - boxWidth / 2;
                
                setConsoleColor(14);
                printBox(startX, 6, boxWidth, 18);
                
                setConsoleColor(15);
                printCentered("=== HIGH SCORES ===", 8);
                setConsoleColor(7);

                ifstream highscoreFile("highscores.txt");
                if (highscoreFile.is_open()) {
                    string line;
                    int y = 11;
                    int count = 0;
                    while (getline(highscoreFile, line) && count < 10) {
                        setConsoleColor(count == 0 ? 14 : (count < 3 ? 10 : 7));
                        printCentered(line, y++);
                        count++;
                    }
                    highscoreFile.close();
                }
                else {
                    setConsoleColor(8);
                    printCentered("No high scores yet!", 14);
                }

                setConsoleColor(8);
                printCentered("Press any key to continue...", 22);
                setConsoleColor(7);
                _getch();
            }
            break;
        case 5:
            system("cls");
            printDeveloperHeader();
            
            setConsoleColor(10);
            printCentered("T H A N K S   F O R   P L A Y I N G !", centerY);
            setConsoleColor(14);
            printCentered("Developed by: Adil ur Rehman", centerY + 2);
            printCentered("Roll No: 24-CS-760", centerY + 3);
            setConsoleColor(11);
            printCentered("S E E   Y O U   N E X T   T I M E !", centerY + 5);
            
            playSound(523, 200);
            playSound(659, 200);
            playSound(784, 400);
            
            saveStatistics();
            saveHighScore();
            Sleep(3000);
            showConsoleCursor();
            exit(0);
            break;
    }
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

    int boxWidth = 35;
    int startX = centerX - boxWidth / 2;
    printBox(startX, 12, boxWidth, 8);

    printCentered("Choose your move:", 14);
    printCentered("'R' for ROCK", 15);
    printCentered("'P' for PAPER", 16);
    printCentered("'S' for SCISSORS", 17);

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
                playSound(500, 100);
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
    printDeveloperHeader();

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
    for (int i = 0; i < 30; i++) cout << char(205);

    gotoxy(width / 2 - 1, 12);
    SetConsoleTextAttribute(console, 12);
    cout << "VS";
    SetConsoleTextAttribute(console, 7);
}

void singlePlayerGame() {
    int player1Score = 0, player2Score = 0;
    int rounds = 1;
    const int maxRounds = 5;
    int roundsWon = 0;

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
        int prevPlayer1Score = player1Score;
        gameProcessing(player1Throw, aiThrow, player1Score, player2Score, isDraw, resultMsg);

        // Track wins for streak
        if (!isDraw && player1Score > prevPlayer1Score + 5) {
            roundsWon++;
        }

        // Display result
        gotoxy(width / 2 - static_cast<int>(resultMsg.length()) / 2, 25);
        if (isDraw) {
            SetConsoleTextAttribute(console, 14); // Yellow for draw
            playSound(400, 200);
        }
        else if (player1Score > player2Score) {
            SetConsoleTextAttribute(console, 10); // Green for win
            playSound(800, 200);
        }
        else {
            SetConsoleTextAttribute(console, 12); // Red for loss
            playSound(300, 200);
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
            playSound(300, 100);
            Sleep(1000);
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 2...";
            playSound(400, 100);
            Sleep(1000);
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 1...";
            playSound(500, 100);
            Sleep(1000);
        }
    }

    // Update statistics
    totalGamesPlayed++;
    if (player1Score > player2Score) {
        totalWins++;
        currentStreak++;
        if (currentStreak > bestStreak) bestStreak = currentStreak;
    } else if (player2Score > player1Score) {
        totalLosses++;
        currentStreak = 0;
    } else {
        totalDraws++;
    }
    
    saveStatistics();

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
        gotoxy(width / 2 - 25, 27);
        cout << currentUsername << "'s turn (screen will clear for privacy)";
        Sleep(2000);
        char player1Throw = getPlayerThrow(1, true, currentUsername);
        system("cls");
        printDeveloperHeader();

        // Player 2 turn
        gotoxy(width / 2 - 25, 27);
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

        // Display result with sound
        gotoxy(width / 2 - static_cast<int>(resultMsg.length()) / 2, 25);
        if (isDraw) {
            SetConsoleTextAttribute(console, 14);
            playSound(400, 200);
        }
        else if (player1Score > player2Score) {
            SetConsoleTextAttribute(console, 10);
            playSound(800, 200);
        }
        else {
            SetConsoleTextAttribute(console, 12);
            playSound(300, 200);
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
            playSound(300, 100);
            Sleep(1000);
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 2...";
            playSound(400, 100);
            Sleep(1000);
            gotoxy(width / 2 - 10, 27);
            cout << "Next round in 1...";
            playSound(500, 100);
            Sleep(1000);
        }
    }

    stopTimer(); // Ensure timer is stopped
    showEndGame(player1Score, player2Score, true, currentUsername, player2Name);
}

void showEndGame(int player1Score, int player2Score, bool isTwoPlayer, string p1Name, string p2Name) {
    system("cls");
    getScreenDimensions();
    
    int startY = centerY - 12;
    
    // ASCII Art Game Over
    setConsoleColor(12); // Red
    printCentered("  ________                        ________                     ", startY);
    printCentered(" /  _____/_____    _____   ____   \\_____  \\___  __ ___________ ", startY + 1);
    printCentered("/   \\  ___\\__  \\  /     \\_/ __ \\   /  ____/\\  \\/ // __ \\_  __ \\", startY + 2);
    printCentered("\\    \\_\\  \\/ __ \\|  Y Y  \\  ___/  /       \\ \\   /\\  ___/|  | \\/", startY + 3);
    printCentered(" \\______  (____  /__|_|  /\\___  > \\_______ \\ \\_/  \\___  >__|   ", startY + 4);
    printCentered("        \\/     \\/      \\/     \\/          \\/         \\/        ", startY + 5);

    // Use provided names or defaults
    if (p1Name.empty()) p1Name = currentUsername;
    if (p2Name.empty()) p2Name = isTwoPlayer ? player2Name : "COMPUTER";

    // Display final scores with player names
    setConsoleColor(14);
    printCentered("=== FINAL SCORES ===", startY + 8);
    
    setConsoleColor(11);
    printCentered(p1Name + ": " + to_string(player1Score) + " points", startY + 10);
    
    setConsoleColor(isTwoPlayer ? 13 : 10);
    printCentered(p2Name + ": " + to_string(player2Score) + " points", startY + 11);

    // Determine winner with sound
    setConsoleColor(15);
    if (player1Score > player2Score) {
        printCentered("*** " + p1Name + " WINS THE CHAMPIONSHIP! ***", startY + 13);
        playSound(523, 200);
        playSound(659, 200);
        playSound(784, 200);
        playSound(1047, 400);
    }
    else if (player2Score > player1Score) {
        printCentered("*** " + p2Name + " WINS THE CHAMPIONSHIP! ***", startY + 13);
        playSound(392, 200);
        playSound(330, 200);
        playSound(262, 400);
    }
    else {
        printCentered("*** IT'S A TIE! ***", startY + 13);
        playSound(440, 400);
    }

    // Save high score for single player
    if (!isTwoPlayer && player1Score > 0) {
        ofstream highscoreFile("highscores.txt", ios::app);
        time_t now = time(nullptr);
        char* timeStr = ctime(&now);
        string timeString(timeStr);
        timeString = timeString.substr(0, timeString.length() - 1); // Remove newline
        highscoreFile << p1Name << " - Score: " << player1Score << " | " << timeString << endl;
        highscoreFile.close();
        
        // Update high score if needed
        if (player1Score > highScore) {
            highScore = player1Score;
            saveHighScore();
            
            setConsoleColor(10);
            printCentered("*** NEW HIGH SCORE! ***", startY + 15);
            playSound(1047, 300);
        }
    }

    setConsoleColor(7);

    // Menu with arrow key navigation
    string options[] = {
        "  Play Again  ",
        "  Main Menu   ",
        "  Exit Game   "
    };
    
    int choice = navigateMenu(options, 3, "What would you like to do?", startY + 18);

    stopTimer(); // Ensure timer is stopped

    switch (choice) {
    case 0:
        player2Name = ""; // Reset player 2 name for two player
        if (isTwoPlayer) twoPlayerGame();
        else singlePlayerGame();
        break;
    case 1:
        player2Name = ""; // Reset player 2 name
        return;
    case 2:
        system("cls");
        printDeveloperHeader();
        
        setConsoleColor(10);
        printCentered("T H A N K S   F O R   P L A Y I N G !", centerY);
        setConsoleColor(14);
        printCentered("Developed by: Adil ur Rehman", centerY + 2);
        printCentered("Roll No: 24-CS-760", centerY + 3);
        setConsoleColor(11);
        printCentered("S E E   Y O U   N E X T   T I M E !", centerY + 5);
        
        playSound(523, 200);
        playSound(659, 200);
        playSound(784, 400);
        
        saveStatistics();
        saveHighScore();
        Sleep(3000);
        showConsoleCursor();
        exit(0);
    }
}

int main() {
    // Set console window title
    system("title ROCK PAPER SCISSORS CHAMPIONSHIP - By Adil ur Rehman");
    
    // Maximize console window
    setConsoleSize();

    // Hide cursor
    hideConsoleCursor();

    // Show loading screen
    printLoadingScreen();

    // Login
    login();

    // Main game loop
    while (true) {
        showMainMenu();
    }

    return 0;
}
