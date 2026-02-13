#include <iostream>
#include <string>

#include "game/board.h"
#include "game/tetromino.h"

Board::Board(std::uint16_t w, std::uint16_t h) : width(w), height(h)
{
    grid.resize(h);
    for (int row = 0; row < h; ++row)
    {
        grid[row].resize(w);
        for (int col = 0; col < w; ++col)
            grid[row][col] = " ";
    }

    Board::bounds = {0, static_cast<std::uint16_t>(w - 1), 0,
                     static_cast<std::uint16_t>(h - 1)};
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
                if (piece.matrix[row - piece.y.begin][col - piece.x.begin] ==
                    "■")
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
                if (piece.matrix[row - piece.y.begin][col - piece.x.begin] ==
                    "■")
                {
                    grid[row][col] = "■";
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
        if (grid[0][col] == "■")
        {
            running = false;
            break;
        }
    }
}

void Board::calculateBaseline(Tetromino& piece)
{
    baseline.clear();

    bool done = false;
    for (auto row = piece.y.end; row >= piece.y.begin; --row)
    {
        for (auto col = piece.x.begin; col <= piece.x.end; ++col)
        {
            if (piece.matrix[row - piece.y.begin][col - piece.x.begin] == "■")
            {
                std::int16_t r = row + 1;
                baseline.push_back({col, r});
                if (col >= piece.x.end) done = true;
            }
        }
        if (done) break;
    }

    for (auto& coord : baseline)
    {
        while (coord.y < height && grid[coord.y][coord.x] == " ")
            coord.y++;
        coord.y--; // step back to the last empty cell
    }
}

void Board::render(Tetromino& piece)
{
    std::cout << "\n\n";
    for (int row = 0; row < height; ++row)
    {
        // left border
        if (row >= 2) std::cout << "│";
        else std::cout << " ";

        for (int col = 0; col < width; ++col)
        {
            std::string cell = grid[row][col];

            if (row >= piece.y.begin && row <= piece.y.end &&
                col >= piece.x.begin && col <= piece.x.end)
            {
                int y = row - piece.y.begin;
                int x = col - piece.x.begin;
                if (piece.matrix[y][x] == "■") cell = "■";
            }

            std::cout << cell;
        }

        // right border
        if (row >= 2) std::cout << "│\n";
        else std::cout << "\n";
    }
    std::cout << " —————————— " << '\n';
}
