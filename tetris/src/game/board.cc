#include <iostream>
#include <string>

#include "core/debugger.h"
#include "core/terminal.h"
#include "game/board.h"
#include "game/tetromino.h"

Board::Board(std::uint16_t w, std::uint16_t h)
{
    terminal::init();

    width  = w > 0 ? w : terminal::WIDTH;
    height = h > 0 ? h : terminal::HEIGHT;

    grid.resize(height);
    for (int row = 0; row < height; ++row)
    {
        grid[row].resize(width);
        for (int col = 0; col < width; ++col)
            grid[row][col] = " ";
    }

    Board::bounds = {0, static_cast<std::int16_t>(width - 1), 0,
                     static_cast<std::int16_t>(height - 1)};
}

void Board::clearLines()
{
    for (int row = height - 1; row >= 0; --row)
    {
        bool fullLine = true;
        for (std::uint16_t col = 0; col < width; ++col)
        {
            if (grid[row][col] == " ")
            {
                fullLine = false;
                break;
            }
        }

        if (fullLine)
        {
            // Move all rows above down by one
            for (int r = row; r > 0; --r)
            {
                grid[r] = grid[r - 1];
            }
            // Clear the top row
            grid[0].assign(width, " ");
            // Check the same row again since it now contains the above row
            row++;
        }
    }
}

void Board::lockPiece(Tetromino& piece)
{
    if (baseline.empty()) return;

    bool shouldLock = false;
    for (const auto& coords : baseline)
    {
        for (auto row = piece.y.end; row >= piece.y.begin; --row)
        {
            for (auto col = piece.x.begin; col <= piece.x.end; ++col)
            {
                if (piece.matrix[row - piece.y.begin][col - piece.x.begin] !=
                    " ")
                {
                    if (coords.x == col && coords.y == row)
                    {
                        shouldLock = true;
                        break;
                    }
                }
            }
            if (shouldLock) break;
        }
        if (shouldLock) break;
    }

    if (shouldLock)
    {
        for (auto row = piece.y.end; row >= piece.y.begin; --row)
        {
            for (auto col = piece.x.begin; col <= piece.x.end; ++col)
            {
                if (piece.matrix[row - piece.y.begin][col - piece.x.begin] !=
                    " ")
                {
                    grid[row][col] =
                        piece.matrix[row - piece.y.begin][col - piece.x.begin];
                }
            }
        }

        baseline.clear();
        piece.spawn();
    }
}

void Board::gameOver(bool& running)
{
    for (std::uint16_t col = 0; col < Board::bounds.RIGHT; ++col)
    {
        if (grid[0][col] != " ")
        {
            running = false;
            break;
        }
    }
}

void Board::calculateBaseline(Tetromino& piece)
{
    piece.calculateBaseline();
    baseline = piece.baseline;

    for (auto& coord : baseline)
    {
        while (coord.y < height && grid[coord.y][coord.x] == " ")
            coord.y++;
        coord.y--; // step back to the last empty cell
    }
}

void Board::render(Tetromino& piece)
{
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            std::string cell = grid[row][col];

            if (row >= piece.y.begin && row <= piece.y.end &&
                col >= piece.x.begin && col <= piece.x.end)
            {
                int y = row - piece.y.begin;
                int x = col - piece.x.begin;
                if (piece.matrix[y][x] != " ") cell = piece.matrix[y][x];
            }

            // for (const auto& coord : baseline)
            // {
            //     if (coord.x == col && coord.y == row)
            //     {
            //         cell = "x";
            //         break;
            //     }
            // }

            std::cout << cell;
        }
        // std::cout << "\n";
    }
    std::cout << std::flush;
}

Coords Board::getLowerBound()
{
    if (baseline.empty()) return {0, 0};

    Coords lowerBound = {baseline[0].x, baseline[0].y};
    for (size_t i = 1; i < baseline.size(); ++i)
    {
        if (lowerBound.x > baseline[i].x) lowerBound.x = baseline[i].x;
        if (lowerBound.y > baseline[i].y) lowerBound.y = baseline[i].y;
    }
    return lowerBound;
}

Coords Board::getUpperBound()
{
    if (baseline.empty()) return {0, 0};

    Coords upperBound = {baseline[0].x, baseline[0].y};
    for (size_t i = 1; i < baseline.size(); ++i)
    {
        if (upperBound.x < baseline[i].x) upperBound.x = baseline[i].x;
        if (upperBound.y < baseline[i].y) upperBound.y = baseline[i].y;
    }
    return upperBound;
}
