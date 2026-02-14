#pragma once

#ifndef TETROMINO_H
#define TETROMINO_H

#include <core/coords.h>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

using matrix_trot = std::array<std::array<std::array<short, 2>, 4>, 4>;
using matrix_t    = std::array<std::array<std::string, 4>, 4>;

enum Shape : std::uint8_t
{
    L = 0,
    T,
    J,
    S,
    Z,
    O,
    I
};

// Colored blocks using ANSI escape codes, indexed by Shape enum
// L=Orange, T=Pink, J=Purple/DarkBlue, S=Green, Z=Red, O=Yellow, I=Cyan
inline const std::array<std::string, 7> coloredBlocks = {
    "\x1b[38;5;208m■\x1b[0m", // L - Orange
    "\x1b[35m■\x1b[0m",       // T - Pink
    "\x1b[34m■\x1b[0m",       // J - Purple/Dark Blue
    "\x1b[32m■\x1b[0m",       // S - Green
    "\x1b[31m■\x1b[0m",       // Z - Red
    "\x1b[33m■\x1b[0m",       // O - Yellow
    "\x1b[36m■\x1b[0m",       // I - Cyan
};

class Tetromino
{
  public:
    Axis x;
    Axis y;
    Shape shapeType;
    int rotationIndex;
    float speed;
    matrix_t matrix;

    Tetromino();

    void spawn();
    void rotate(const std::vector<Coords>& baseline, bool clockwise = true);
    void fallLoop(double& dt, double& gravityInterval, double& gravityTimer);
    void moveLeft(const std::vector<Coords>& baseline);
    void moveRight(const std::vector<Coords>& baseline);
    void hardDrop(const std::vector<Coords>& baseline);
    void softDrop();
    void wallKick();
};

#endif // TETROMINO_H
