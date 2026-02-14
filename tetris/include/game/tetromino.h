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

// Colored blocks loaded from assets/textures/blocks, indexed by Shape enum
extern const std::array<std::string, 7> coloredBlocks;

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
