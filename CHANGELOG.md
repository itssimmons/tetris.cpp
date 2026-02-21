# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Planned
- Score tracking and high-score persistence
- Next-piece preview panel
- Pause / resume functionality
- Sound integration using the bundled `soundtrack.ogg`
- Color support via ANSI 256-color or truecolor escape codes
- Windows support (currently experimental in CI)

---

## [0.1.0-beta] - 2025-01-01

### Added

#### Core engine
- `Engine` class with a fixed-timestep game loop targeting **30 FPS**
- Frame-time capping via `std::this_thread::sleep_for` to avoid busy-waiting
- Clean shutdown on `ESC` key press

#### Rendering
- Terminal-based renderer using **ANSI escape codes** (`ansi` namespace)
  - Cursor hide / restore, home-cursor, raw mode, and non-blocking I/O
- `Board::render` draws the full grid and the active tetromino each frame

#### Keyboard input
- Non-blocking keyboard polling through the `Keyboard` class
- Supported keys:
  | Key | Action |
  |-----|--------|
  | Arrow Up / `X` | Rotate clockwise |
  | `Z` | Rotate counter-clockwise |
  | Arrow Down | Soft drop (5× speed boost) |
  | Spacebar | Hard drop |
  | Arrow Left | Move left |
  | Arrow Right | Move right |
  | `ESC` | Quit game |

#### Game logic — Board
- Dynamic `Board` sized to the current terminal dimensions (`terminal::init`)
- Static `bounds_t` struct tracking LEFT / RIGHT / TOP / BOTTOM boundaries
- **Line clearing**: completed rows are removed and rows above shift down
- **Piece locking**: pieces are locked onto the grid when they reach the baseline
- **Game over**: detected when a locked block occupies the top row

#### Game logic — Tetromino
- All 7 standard Tetris pieces: **L, T, J, S, Z, O, I**
- Random piece selection via a dedicated `RNG` utility
- Block textures loaded from `tetris/assets/textures/blocks` at startup
- **Gravity loop**: time-based falling speed (`gravityInterval = 0.6 s`),
  driven by the engine delta-time
- **Clockwise and counter-clockwise rotation** using per-shape rotation tables
- **Wall kicks**: pieces are nudged inward when rotation would push them
  outside the board boundaries
- **Hard drop**: instantly snaps the piece to its lowest valid position
- **Soft drop**: temporarily raises fall speed to 15×
- **Baseline calculation**: per-column lowest-occupied cell used by locking
  and hard-drop logic

#### Developer tooling
- **Debug logger** (`dbg::start` / `dbg::stop` / `dbg::log`) writing to `debug.log`
- `Debugger` module logs baseline coordinates each frame during development
- **Bazel** build system (`BUILD.bazel`) with `C++20` standard
- `.clang-format` (LLVM-based style) and `.clang-tidy` for code quality
- `.editorconfig` for consistent editor settings across contributors

#### CI / CD
- **GitHub Actions — Bazel Build** workflow triggered on version tags (`v*`)
  - Builds on `macos-latest` and `ubuntu-latest` (required)
  - Builds on `windows-latest` (experimental / `continue-on-error`)
  - Uploads platform-specific binaries as workflow artifacts
- **GitHub Actions — Create Release** workflow
  - Runs automatically after a successful build
  - Publishes a GitHub Release with all platform artifacts attached

#### Assets
- `tetris/assets/textures/blocks` — comma-separated block glyphs for the 7 piece types
- `tetris/assets/audio/soundtrack.ogg` — bundled game soundtrack (not yet wired up)

---

[Unreleased]: https://github.com/itssimmons/tetris.cpp/compare/v0.1.0-beta...HEAD
[0.1.0-beta]: https://github.com/itssimmons/tetris.cpp/releases/tag/v0.1.0-beta
