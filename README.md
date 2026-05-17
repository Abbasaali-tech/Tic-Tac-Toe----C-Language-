# Tic Tac Toe Game 🎮

A complete **Tic Tac Toe Game in C/C++** with two versions:

1. **Console-Based Version (C++)**
2. **GUI Version using Raylib Library**

The project includes:

* Login & Signup system
* Player vs Player mode
* Player vs Computer (AI using Minimax)
* Score tracking
* Match history saving
* Replay functionality

---

## 📌 Features

### ✅ Console Version

* Login and Signup authentication
* Play against a friend or computer
* Smart AI using Minimax Algorithm
* Score tracking system
* Match history storage
* Input validation
* Replay option

### ✅ GUI Version (Raylib)

* Interactive graphical interface
* Mouse-based gameplay
* Login/Signup screens
* Symbol selection
* AI opponent
* Game history viewer
* Smooth board rendering using Raylib

---

# 🛠 Technologies Used

* **C / C++**
* **Raylib Graphics Library**
* File Handling
* Structures
* Arrays
* Functions
* Minimax Algorithm

---

# 📂 Project Structure

```bash
TicTacToe/
│
├── Console_Version.cpp
├── GUI_Version.cpp
├── credentials.txt
├── history.txt
└── README.md
```

---

# ▶️ How to Run

## 1️⃣ Console Version

### Compile

```bash
g++ Console_Version.cpp -o tictactoe
```

### Run

```bash
./tictactoe
```

---

## 2️⃣ GUI Version (Raylib)

### Requirements

Install Raylib library first.

### Compile

```bash
g++ GUI_Version.cpp -o game -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Run

```bash
./game
```

---

# 🧠 AI Algorithm

The computer opponent uses the **Minimax Algorithm**, which:

* Evaluates all possible moves
* Chooses the best move
* Makes the AI unbeatable

---

# 💾 File Handling

## credentials.txt

Stores:

```txt
username password
```

## history.txt

Stores:

```txt
winner moves
```

---

# 🎯 Gameplay

* Players choose their symbols.
* Take turns placing symbols on the board.
* First player to align 3 symbols wins.
* Draw is declared if the board becomes full.

---

# 📸 GUI Screens

The GUI version contains:

* Login Screen
* Signup Screen
* Mode Selection
* Symbol Selection
* Game Board
* Post Game Screen
* History Screen

---

# 🚀 Future Improvements

* Online Multiplayer
* Difficulty Levels
* Sound Effects
* Animations
* Leaderboard System
* Database Integration

---

# 👨‍💻 Authors

Developed as an Object-Oriented / Programming Fundamentals project.

---

# 📜 License

This project is for educational purposes only.
