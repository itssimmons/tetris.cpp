# Copilot Instructions: Tetris.cpp — Console Tetris Game (C++20)

## Repository
**Repository Name**: [itssimmons/tetris.cpp on GitHub](https://github.com/itssimmons/tetris.cpp)  
**Description**: Command‑line Tetris game made with modern C++ (C++20), rendered in text/ANSI in terminal. Supports gameplay mechanics (tetromino spawn, rotation, line clearing, scoring), real‑time input, and basic console UI rendering.

## Purpose
These instructions help GitHub Copilot assist with tasks related to the **Tetris.cpp** project. They provide context for understanding the code structure, offering improvements, explaining game logic, generating tests, fixing bugs, writing new features, and documenting installation/build instructions.

---

## Language & Technology
- **Language**: C++ (modern, C++20)
- **Type**: Console/terminal game.
- **Rendering**: Text/ANSI UI in terminal.
- **Build Tools**: `g++`, Bazel, and other CLI build systems (Make, etc.).

---

## What Copilot Should Help With

### 🧠 Code Understanding
- **Describe architecture**: Explain game design (game loop, board representation, tetromino logic).
- **Explain functions**: Detail logic for rotation, collision detection, line clearing, score system.
- **Clarify C++20 features**: Identify and explain use of modern C++ features (concepts, ranges, coroutines, constexpr, etc.) if used.

### 💡 Development Assistance
- **Help with features**  
  Example tasks Copilot can assist with:
  - Add **hold piece** functionality (store one tetromino).
  - Add **next pieces preview** UI.
  - Implement **pause/resume** and **sound effects** (console beeps).
  - Add **high score persistence** (file save/load).
- **Refactor code**
  - Improve board representation with enums or strong types.
  - Modularize rendering and game logic into classes/headers.
- **Bug fixing**
  - Fix rotation glitches.
  - Improve collision handling at edges.
  - Address scoring anomalies.

### 📦 Build & Packaging
- Provide development setup instructions for:
  - **Linux** builds (`g++ -std=c++20 ...`).
  - **Windows** builds (MinGW, Visual Studio).
  - **Cross‑platform terminal support** (ncurses, ANSI handling).
- Suggest CMake/Makefile templates.

### 🧪 Testing
- Generate unit tests (e.g., using GoogleTest) for:
  - Rotation functions.
  - Collision checks.
  - Scoring logic.

### 📘 Documentation/Comments
- Improve `README.md` with:
  - Clear installation steps.
  - Gameplay controls.
  - Contribution guidelines.
  - License explanation.

---

## Example Prompts Copilot Should Handle

### Code & Logic
**User asks:**
- "Explain how the tetromino rotation works."
- "Why do pieces collide incorrectly at the right edge?"
- "Add a feature to preview next 3 pieces."

**Expected Copilot Action:**
- Provide a detailed explanation of the rotation math.
- Identify potential out‑of‑bounds checks.
- Provide code snippet to implement preview.

---

## Commands to Understand

### Gameplay Commands
These are NOT literal system commands, but understanding semantics is useful:
- `move left`, `move right`, `rotate piece`, `soft drop`, `hard drop`, etc.

### Build Commands
- `g++ -std=c++20 -o tetris main.cpp …`
- Generate CMake/Makefile templates.

---

## Build & Run (Example Instructions)

```sh
# Clone the repo
git clone https://github.com/itssimmons/tetris.cpp

# Navigate
cd tetris.cpp

# Compile (Linux)
g++ -std=c++20 -O2 -o tetris main.cpp game.cpp board.cpp renderer.cpp

# Run
./tetris
```
