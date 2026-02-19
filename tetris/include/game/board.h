#pragma once

#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <vector>

#include <core/coords.h>
#include <game/tetromino.h>

struct bounds_t
{
    std::int16_t LEFT;
    std::int16_t RIGHT;
    std::int16_t TOP;
    std::int16_t BOTTOM;
};

class Board
{
  public:
    inline static bounds_t bounds;

    std::uint16_t width;
    std::uint16_t height;
    std::vector<Coords> baseline;
    std::vector<std::vector<std::string>> grid;

    Board(std::uint16_t w = 0, std::uint16_t h = 0);

    void clearLines();
    void lockPiece(Tetromino& piece);
    void gameOver(bool& running);
    void render(Tetromino& piece);

    void calculateBaseline(Tetromino& piece);
    Coords getLowerBound();
    Coords getUpperBound();
};

#endif // BOARD_H
