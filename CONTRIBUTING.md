# Contributing to tetris.cpp

Thank you for your interest in contributing! This document explains how to set
up your environment, the skills you will need, and the conventions to follow
when submitting changes.

---

## Table of Contents

1. [Required Skills](#required-skills)
2. [Prerequisites](#prerequisites)
3. [Building the Project](#building-the-project)
4. [Code Style](#code-style)
5. [Project Structure](#project-structure)
6. [How to Contribute](#how-to-contribute)
7. [CI / CD](#ci--cd)

---

## Required Skills

| Skill | Why it matters |
|-------|----------------|
| **C++20** | The entire codebase is written in C++20 (concepts, ranges, `std::chrono` literals, designated initialisers, etc.) |
| **Terminal / ANSI escape codes** | Rendering is done entirely through ANSI sequences — no GUI library is used |
| **Game-loop design** | The engine uses a fixed-timestep loop; understanding delta-time and frame-capping is important |
| **Bazel build system** | All build targets are declared in `BUILD.bazel`; familiarity with `cc_binary` / `cc_library` rules is needed |
| **Git & GitHub** | Contributions are submitted as pull requests against the `main` branch |

Nice-to-have experience:
- Tetris / falling-piece game mechanics (rotation tables, wall kicks, gravity)
- `clang-format` and `clang-tidy` for automated code formatting and linting
- GitHub Actions for CI/CD workflows

---

## Prerequisites

| Tool | Version | Notes |
|------|---------|-------|
| C++ compiler | GCC ≥ 13 or Clang ≥ 16 | Must support C++20 |
| [Bazelisk](https://github.com/bazelbuild/bazelisk) | latest | Wrapper that automatically downloads the Bazel version specified in `.bazelversion` |
| Go | ≥ 1.21 | Required to install Bazelisk via `go install` |
| `clang-format` | ≥ 16 | Code formatting (config in `.clang-format`) |
| `clang-tidy` | ≥ 16 | Static analysis (config in `.clang-tidy`) |

### Installing Bazelisk

```bash
go install github.com/bazelbuild/bazelisk@latest
export PATH="$(go env GOPATH)/bin:$PATH"
```

---

## Building the Project

```bash
# Build the tetris binary
bazel build //tetris:tetris --verbose_failures

# Run the game directly from the Bazel output
$(bazel info bazel-bin)/tetris/tetris
```

The build will produce a self-contained binary alongside a `.runfiles/`
directory that includes the block-texture asset required at runtime.

---

## Code Style

All C++ source files must be formatted with **clang-format** before being
committed. The style is configured in `.clang-format` (LLVM-based, Allman
braces, 4-space indentation, 80-column limit).

```bash
# Format a single file in-place
clang-format -i tetris/src/game/board.cc

# Format all C++ sources
find tetris/src tetris/include -name '*.cc' -o -name '*.h' \
  | xargs clang-format -i
```

Static-analysis checks are defined in `.clang-tidy`. Address any reported
warnings before opening a pull request.

Key style points:
- **Allman brace style** — opening braces on their own line
- **4-space indentation**, no tabs
- **`#pragma once`** guard plus matching `#ifndef` / `#define` / `#endif` in every header
- **Left-aligned pointers and references** (`int* p`, `int& r`)
- **`auto` return types** for non-trivial return types where it improves readability
- **`std::uint*_t` / `std::int*_t`** fixed-width integer types preferred

---

## Project Structure

```
tetris.cpp/
├── tetris/
│   ├── BUILD.bazel              # Bazel build rules
│   ├── assets/
│   │   ├── audio/
│   │   │   └── soundtrack.ogg  # Bundled game soundtrack
│   │   └── textures/
│   │       └── blocks          # Comma-separated block glyphs (7 entries)
│   ├── include/
│   │   ├── core/
│   │   │   ├── ansi.h          # ANSI escape-code utilities & key enum
│   │   │   ├── coords.h        # Coords / Axis structs
│   │   │   ├── debugger.h      # File-based debug logger
│   │   │   ├── engine.h        # Engine class & timing constants
│   │   │   ├── keyboard.h      # Non-blocking keyboard input
│   │   │   ├── rng.h           # Random-number generator
│   │   │   └── terminal.h      # Terminal size detection
│   │   └── game/
│   │       ├── board.h         # Board class (grid, line clear, locking)
│   │       └── tetromino.h     # Tetromino class (shapes, rotation, movement)
│   └── src/
│       ├── core/               # Implementations of core headers
│       ├── game/               # Implementations of game headers
│       └── main.cc             # Entry point — calls Engine::run()
├── .clang-format
├── .clang-tidy
├── .editorconfig
├── .bazelversion
├── MODULE.bazel
├── CHANGELOG.md
└── CONTRIBUTING.md
```

---

## How to Contribute

1. **Fork** the repository and create a feature branch from `main`:
   ```bash
   git checkout -b feat/my-new-feature
   ```
2. **Make your changes** following the code style described above.
3. **Format** all modified C++ files with `clang-format`.
4. **Build** the project and verify the game runs correctly:
   ```bash
   bazel build //tetris:tetris --verbose_failures
   ```
5. **Commit** with a descriptive message following
   [Conventional Commits](https://www.conventionalcommits.org/):
   ```
   feat(game): add next-piece preview panel
   fix(board): correct off-by-one in line-clear row shift
   ```
6. **Open a Pull Request** against `main` and fill in the PR template.
7. Address any review feedback and wait for CI to pass before merging.

### Commit Message Convention

```
<type>(<scope>): <short summary>

[optional body]
```

Common types: `feat`, `fix`, `refactor`, `docs`, `ci`, `chore`, `test`  
Common scopes: `engine`, `board`, `tetromino`, `ansi`, `keyboard`, `rng`, `ci`

---

## CI / CD

Pull requests are validated by the **Bazel Build** GitHub Actions workflow,
which builds the project on macOS, Ubuntu, and Windows.  Windows support is
currently experimental (`continue-on-error: true`).

When a version tag (`v*`) is pushed, the workflow also runs and — on success —
the **Create Release** workflow automatically publishes a GitHub Release with
platform binaries attached.

Make sure your changes build successfully on both **macOS** and **Ubuntu**
before requesting a merge.
