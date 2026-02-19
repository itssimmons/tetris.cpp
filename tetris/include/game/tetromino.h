#pragma once

#ifndef TETROMINO_H
#define TETROMINO_H

#include <core/coords.h>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

using matrix_t = std::array<std::array<std::string, 4>, 4>;

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

extern const std::array<std::string, 7> blocks;

class Tetromino
{
  public:
    Axis x;
    Axis y;
    Shape shapeType;
    int rotationIndex;
    float speed;
    matrix_t matrix;
    std::vector<Coords> baseline;

    Tetromino();

    void spawn();
    void rotate(const std::vector<Coords>& boardBaseline,
                bool clockwise = true);
    void fallLoop(double& dt, const std::vector<Coords>& boardBaseline);
    void moveLeft(std::vector<std::vector<std::string>> grid);
    void moveRight(std::vector<std::vector<std::string>> grid);
    void hardDrop(const std::vector<Coords>& boardBaseline);
    void softDrop();
    void wallKick(Axis& refX, Axis& refY);

    /// The baseline of a tetromino isn't more than a set of coordinates
    /// representing where are the 4 blocks of 4x4 matrix () in the board.
    void calculateBaseline();
    Coords getLowerBound();
    Coords getUpperBound();
};

#endif // TETROMINO_H
