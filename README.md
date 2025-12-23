# 🪨📄✂️ Rock Paper Scissors Championship (Console Edition)

> A fully interactive **C++ console-based game** that transforms a simple concept into a competitive, time-bound championship experience.

This project is not just about Rock–Paper–Scissors.  
It is about **game logic**, **real-time input handling**, **multi-threading**, **console UI engineering**, and **user authentication**—all inside a Windows terminal.

---

## 🎯 Project Overview

**Rock Paper Scissors Championship** is a feature-rich C++ console application supporting **single-player (vs AI)** and **two-player competitive modes**.  
It includes a login system, countdown timer, score tracking, ASCII-based UI, and persistent high-score storage.

Every round is timed.  
Every move matters.  
Every second counts.

---

## 🧠 Core Features

### 🔐 Account System
- User **registration & login**
- Credential storage using file handling
- Masked password input for security

### ⏱️ Real-Time Timer (Multi-threaded)
- 30-second countdown per move
- Color-coded timer feedback:
  - 🟢 Safe time
  - 🟡 Warning
  - 🔴 Critical
- Automatic move selection if time expires

### 🎮 Game Modes
- **Single Player** (vs Computer AI)
- **Two Player Mode** (local competitive play with privacy handling)

### 🧮 Scoring System
- Win → **10 points**
- Draw → **5 points each**
- Best-of-5 rounds per match

### 🖥️ Console UI & UX
- Center-aligned dynamic layout
- ASCII art for Rock / Paper / Scissors
- Colored text for clarity and feedback
- Custom console font & hidden cursor for immersion

### 🏆 High Score System
- Persistent score storage
- Timestamped records
- Viewable leaderboard inside the app

---

## 🛠️ Tech Stack

| Technology | Usage |
|----------|------|
| **C++** | Core logic & structure |
| **Windows API** | Console control, threading, UI |
| **Multi-threading** | Countdown timer |
| **File Handling** | Login & high scores |
| **Atomic Variables** | Thread-safe timing |
| **ASCII Graphics** | Visual feedback |

---

## 📂 Project Structure (Conceptual)
