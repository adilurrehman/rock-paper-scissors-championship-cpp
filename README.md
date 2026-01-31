# 🪨📄✂️ Rock Paper Scissors Championship (C++)

A captivating C++ console-based competitive game that transforms the classic Rock-Paper-Scissors into an immersive championship experience with modern UI features.

---

## 🎯 Overview

**Rock Paper Scissors Championship** is a Windows-based console application featuring **single-player (vs AI)** and **two-player competitive modes**.

The project focuses on **game logic**, **real-time input handling**, **multi-threading**, **console UI engineering**, and **user authentication**—all inside a Windows terminal.

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| 🔐 **User Authentication** | Registration & login system with password masking |
| 🎮 **Multiple Game Modes** | Single Player (vs AI) and Two Player (Local) |
| ⏱️ **Real-Time Timer** | 30-second countdown per move with color feedback |
| 🏆 **Scoring System** | Points for wins and draws, persistent high scores |
| 📊 **Player Statistics** | Track wins, losses, draws, and win streaks |
| 💾 **Data Persistence** | High scores & stats saved per user account |
| 🎨 **Centered Console UI** | Dynamic centering adapts to console size |
| ⌨️ **Arrow Key Navigation** | Smooth menu navigation with visual highlighting |
| 🔊 **Sound Effects** | Audio feedback for actions and game events |
| 📈 **Progress Tracking** | Win streak tracking with best streak record |
| 🖼️ **ASCII Art Display** | Visual representation of Rock/Paper/Scissors |
| 🎬 **Animated Loading** | Loading bar with sound on startup |

---

## 🛠️ Tech Stack

- **Language:** C++
- **Platform:** Windows API (Console Control)
- **Libraries:** 
  - `windows.h` - Console manipulation, cursor positioning & colors
  - `conio.h` - Real-time keyboard input
  - `fstream` - File handling for user data
  - `atomic` - Thread-safe timer variables
- **Features:** 
  - Multi-threaded countdown timer
  - File-based authentication & data persistence
  - Dynamic screen centering & responsive layout
  - Password masking for security
  - Unicode box drawing characters

---

## 🎮 Game Mechanics

### Game Modes:
| Mode | Description |
|------|-------------|
| 🤖 Single Player | Play against Computer AI |
| 👥 Two Player | Local competitive play with privacy handling |

### Winning Rules:
| Move | Beats |
|------|-------|
| 🪨 ROCK | ✂️ SCISSORS |
| ✂️ SCISSORS | 📄 PAPER |
| 📄 PAPER | 🪨 ROCK |

### Scoring System:
| Action | Points |
|--------|--------|
| Win a round | +10 |
| Draw | +5 each |
| Best of 5 rounds | Per match |

### Statistics Tracked:
| Stat | Description |
|------|-------------|
| Total Games | Number of matches played |
| Wins/Losses/Draws | Game outcomes |
| Current Streak | Consecutive wins |
| Best Streak | Highest win streak achieved |
| Win Rate | Percentage of wins |
| High Score | Best score achieved |

---

## 🚀 How to Run

### Option 1: Compile from Source
1. Open `Rock-Paper-Scissor.cpp` in any C++ IDE (Visual Studio, Code::Blocks, Dev-C++)
2. Build the project
3. Run the compiled executable

### Option 2: Command Line
```bash
g++ Rock-Paper-Scissor.cpp -o RPS-Championship.exe
./RPS-Championship.exe
```

---

## 🎛️ Controls

| Key | Action |
|-----|--------|
| ↑ ↓ | Navigate menu options |
| Enter | Select option / Confirm |
| ESC | Return to previous menu / Exit |
| R | Choose ROCK (during game) |
| P | Choose PAPER (during game) |
| S | Choose SCISSORS (during game) |

---

## 📁 Project Structure

```
Rock-Paper-Scissors-Championship/
├── Rock-Paper-Scissor.cpp   # Main source code (1100+ lines)
├── login.txt                # User credentials & high scores
├── highscores.txt           # Global high score leaderboard
├── stats_[username].txt     # Per-user statistics
└── README.md                # Project documentation
```

---

## 📸 Screenshots

```
╔══════════════════════════════════════════════════════╗
║         ROCK PAPER SCISSORS CHAMPIONSHIP             ║
║   Developed by: Adil ur Rehman | Roll: 24-CS-760     ║
╚══════════════════════════════════════════════════════╝

Player: John                              Score: 25
High Score: 45                            High Score: 45
Win Streak: 3                             Moves: 3

              === MAIN MENU ===

   ╔═════════════════════════════════════════════╗
   ║ > SINGLE PLAYER (vs Computer) <             ║
   ║   TWO PLAYERS (Local)                       ║
   ║   VIEW INSTRUCTIONS                         ║
   ║   VIEW STATISTICS                           ║
   ║   VIEW HIGH SCORES                          ║
   ║   EXIT GAME                                 ║
   ╚═════════════════════════════════════════════╝

         [UP/DOWN: Navigate | ENTER: Select]
```

---

## 🎯 Game Flow

```
┌─────────────────┐
│  Loading Screen │
│  (Animated Bar) │
└────────┬────────┘
         ▼
┌─────────────────┐
│   Login/Register│
└────────┬────────┘
         ▼
┌─────────────────┐
│    Main Menu    │◄──────────────────┐
└────────┬────────┘                   │
         ▼                            │
┌─────────────────┐                   │
│  Select Mode    │───────────────────┤
│ (1P/2P/Stats)   │                   │
└────────┬────────┘                   │
         ▼                            │
┌─────────────────┐                   │
│   Game Round    │                   │
│  (30s Timer)    │                   │
└────────┬────────┘                   │
         ▼                            │
┌─────────────────┐                   │
│ Show Results    │                   │
│ (ASCII Art)     │                   │
└────────┬────────┘                   │
         ▼                            │
┌─────────────────┐    Play Again     │
│   Game Over     │───────────────────┘
│  (Final Score)  │
└────────┬────────┘
         ▼
┌─────────────────┐
│  Save Stats &   │
│   High Score    │
└─────────────────┘
```

---

## ⚠️ Requirements

- **OS:** Windows only (uses WinAPI)
- **Compiler:** Any C++ compiler supporting Windows headers
- **Console:** Windows Command Prompt or PowerShell
- **Sound:** System speaker for audio feedback

---

## 📚 Learning Outcomes

- ✅ Console UI/UX design with dynamic centering
- ✅ File-based authentication system
- ✅ Game state management (scores, stats, streaks)
- ✅ Multi-threaded timer implementation
- ✅ Arrow key input handling
- ✅ Data persistence with file I/O
- ✅ Modular function design
- ✅ Color-coded console output
- ✅ Sound effect integration
- ✅ Unicode box drawing characters
- ✅ Thread-safe programming with atomic variables

---

## 🎨 UI Features

| Feature | Implementation |
|---------|----------------|
| Colored Text | 15+ color combinations for different elements |
| Box Drawing | Unicode box characters for menus |
| Animated Loading | Loading bar with progressive sound |
| Password Masking | Asterisks shown instead of password |
| Centered Layout | All content dynamically centered |
| Highlighted Selection | Inverted colors for selected menu item |
| Timer Display | Color-coded countdown (Green → Yellow → Red) |
| ASCII Art | Hand gesture art for Rock/Paper/Scissors |

---

## 🔊 Sound Design

| Event | Sound |
|-------|-------|
| Loading Progress | Ascending tones |
| Menu Navigation | Click sound |
| Selection Confirm | Confirmation beep |
| Win | Victory melody |
| Loss | Defeat tone |
| Draw | Neutral tone |
| Time's Up | Alert sound |
| New High Score | Celebration melody |

---

## 👨‍💻 Author

**Adil ur Rehman**  
Computer Science Undergraduate  
Roll No: 24-CS-760

---

## 📄 License

This project is created for educational purposes as part of the **Programming Fundamentals** course final semester project.

---

*Made with ❤️ and C++*
