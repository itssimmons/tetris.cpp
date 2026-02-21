# Command-Line Tetris made with C++

![WhatsApp Video 2026-02-21 at 18 29 07](https://github.com/user-attachments/assets/a96a7ef6-0531-49d3-beb8-9f0e187a4616)


This is a simple implementation of the classic Tetris game using C++. The game is played in the terminal, and it features very basic Tetris mechanics such as rotating and moving pieces, clearing lines.

## Table of Contents

- [Requirements to Code](#requirements-to-code)
- [Building the Project](#building-the-project)
- [Running the Game](#running-the-game)
- [Controls](#controls)
- [Features](#features)
- [Future Improvements](#future-improvements)
- [License](#license)
- [Contributing](#contributing)

## Requirements to Code

- A C++ compiler (like g++)
- Terminal or command-line interface (ANSI support for better visuals)
- Bazel build system (optional, but recommended for building the project)
- C++20 or later

## Building the Project

1. Clone the repository:

```bash
git clone https://github.com/itssimmons/tetris.cpp.git
cd tetris.cpp
```

2. Build the project using Bazel:

```bash
bazel build //tetris:tetris
```

## Running the Game

After building the project, you can run the game using the following command:

```bash
bazel run //tetris:tetris
```

## Controls

- <kbd>←</kbd>: Move piece left
- <kbd>→</kbd>: Move piece right
- <kbd>↓</kbd>: Soft drop piece
- <kbd>↑</kbd>: Rotate piece (clockwise)
- <kbd>space</kbd>: Drop piece instantly
- <kbd>esc</kbd>: Exit the game
- <kbd>Z</kbd>: Rotate piece (counter-clockwise)
- <kbd>X</kbd>: Rotate piece (clockwise)

## Features

- Basic Tetris mechanics (moving, rotating, and dropping pieces).
- Line clearing.
- Game over condition when pieces stack up to the top of the playfield.
- The size of the board is based on the size of the terminal window, allowing for a dynamic play area.
- Frame rate of 30 FPS for smooth gameplay in a terminal environment.
- Piece generation using "seed" based randomization for reproducibility.
- ANSI escape codes for better visuals in the terminal.

## Future Improvements

- Implementing a scoring system.
- Accelerating piece speed as the game progresses.
- Implementing a hold piece feature.
- Implementing a next pieces preview.
- Adding sound effects (if possible in a terminal environment).
- Spinning mechanics and T-spin recognition.
- CLI options for customizing the game (e.g., board size, piece colors, base speed, etc...).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

## Contributing

Contributions are welcome! If you have any ideas for improvements or want to fix bugs, feel free to submit a [pull request](https://github.com/itssimmons/tetris.cpp/pulls) or open an [issue](https://github.com/itssimmons/tetris.cpp/issues).
