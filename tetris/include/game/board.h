#pragma once

#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <vector>

#include <core/coords.h>
#include <game/tetromino.h>

constexpr std::uint16_t BOARD_WIDTH  = 10U;
constexpr std::uint16_t BOARD_HEIGHT = 22U; // 20 + 2 (2 hidden rows)

struct bounds_t
{
    size_t LEFT;
    size_t RIGHT;
    size_t TOP;
    size_t BOTTOM;
};

class Board
{
  public:
    inline static bounds_t bounds;

    std::uint16_t width;
    std::uint16_t height;
    std::vector<Coords> baseline;
    std::vector<std::vector<std::string>> grid;

    Board(std::uint16_t w = BOARD_WIDTH, std::uint16_t h = BOARD_HEIGHT);

    void clearLines();
    void lockPiece(Tetromino& piece);
    void gameOver(bool& running);
    void calculateBaseline(Tetromino& piece);
    void render(Tetromino& piece);
};

#endif // BOARD_H
