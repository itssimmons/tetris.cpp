#include <algorithm>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "core/debugger.h"
#include "core/rng.h"
#include "game/board.h"
#include "game/tetromino.h"

static std::array<std::string, 7> loadBlocks()
{
    std::array<std::string, 7> result;
    std::ifstream file("tetris/assets/textures/blocks");
    if (!file.is_open()) return result;

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    std::stringstream ss(content);
    std::string token;
    for (size_t i = 0; i < 7 && std::getline(ss, token, ','); ++i)
        result[i] = token;

    return result;
}

const std::array<std::string, 7> blocks = loadBlocks();

std::unordered_map<Shape, matrix_t> shapes{
    {Shape::L,
     {{{" ", " ", blocks[L], " "},
       {blocks[L], blocks[L], blocks[L], " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::T,
     {{{" ", blocks[T], " ", " "},
       {blocks[T], blocks[T], blocks[T], " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::J,
     {{{blocks[J], " ", " ", " "},
       {blocks[J], blocks[J], blocks[J], " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::S,
     {{{" ", blocks[S], blocks[S], " "},
       {blocks[S], blocks[S], " ", " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::Z,
     {{{blocks[Z], blocks[Z], " ", " "},
       {" ", blocks[Z], blocks[Z], " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::O,
     {{{blocks[O], blocks[O], " ", " "},
       {blocks[O], blocks[O], " ", " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::I,
     {{{" ", " ", " ", " "},
       {blocks[I], blocks[I], blocks[I], blocks[I]},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
};

const std::unordered_map<Shape,
                         std::array<std::array<std::array<short, 2>, 4>, 4>>
    rotations{{Shape::L,
               {{{{{-1, 0}, {0, 0}, {1, 0}, {1, 1}}},
                 {{{0, 1}, {0, 0}, {0, -1}, {1, -1}}},
                 {{{1, 0}, {0, 0}, {-1, 0}, {-1, -1}}},
                 {{{0, -1}, {0, 0}, {0, 1}, {-1, 1}}}}}},
              {Shape::T,
               {{{{{-1, 0}, {0, 0}, {1, 0}, {0, 1}}},
                 {{{0, 1}, {0, 0}, {0, -1}, {1, 0}}},
                 {{{1, 0}, {0, 0}, {-1, 0}, {0, -1}}},
                 {{{0, -1}, {0, 0}, {0, 1}, {-1, 0}}}}}},
              {Shape::J,
               {{{{{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}},
                 {{{0, 1}, {0, 0}, {0, -1}, {1, 1}}},
                 {{{1, 0}, {0, 0}, {-1, 0}, {1, -1}}},
                 {{{0, -1}, {0, 0}, {0, 1}, {-1, -1}}}}}},
              {Shape::S,
               {{{{{-1, 0}, {0, 0}, {0, 1}, {1, 1}}},
                 {{{0, 1}, {0, 0}, {1, 0}, {1, -1}}},
                 {{{1, 0}, {0, 0}, {0, -1}, {-1, -1}}},
                 {{{0, -1}, {0, 0}, {-1, 0}, {-1, 1}}}}}},
              {Shape::Z,
               {{{{{-1, 1}, {0, 1}, {0, 0}, {1, 0}}},
                 {{{1, 1}, {1, 0}, {0, 0}, {0, -1}}},
                 {{{1, -1}, {0, -1}, {0, 0}, {-1, 0}}},
                 {{{-1, -1}, {-1, 0}, {0, 0}, {0, 1}}}}}},
              {Shape::O,
               {{{{{0, 0}, {1, 0}, {0, 1}, {1, 1}}},
                 {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}},
                 {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}},
                 {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}}}}},
              {Shape::I,
               {{{{{-1, 0}, {0, 0}, {1, 0}, {2, 0}}},
                 {{{0, 1}, {0, 0}, {0, -1}, {0, -2}}},
                 {{{-1, 0}, {0, 0}, {1, 0}, {2, 0}}},
                 {{{0, 1}, {0, 0}, {0, -1}, {0, -2}}}}}}};

Tetromino::Tetromino()
{
    rotationIndex = 0;
    speed         = 3.5f;
}

void Tetromino::spawn()
{
    RNG rng;
    size_t r  = rng::randIndex(rng, shapes.size());
    shapeType = static_cast<Shape>(r);
    matrix    = shapes.at(shapeType);

    int xBeginCalc = static_cast<std::int16_t>(
        std::floor((Board::bounds.RIGHT - 1.0) / 2.0) -
        std::floor((matrix[0].size() - 1.0) / 2.0));
    x.begin = static_cast<std::int16_t>(std::max(0, xBeginCalc));
    x.end   = static_cast<std::int16_t>(x.begin + matrix[0].size() - 1);
    y.begin = 0;
    y.end   = static_cast<std::int16_t>(y.begin + matrix.size() - 1);

    rotationIndex = 0;
}

double gravityTimer    = 0.0;
double gravityInterval = 0.6f;

void Tetromino::fallLoop(double& dt, const std::vector<Coords>& boardBaseline)
{
    double currentInterval  = gravityInterval;
    gravityTimer           += dt * speed;

    while (gravityTimer >= currentInterval)
    {
        const auto lowerBound = getLowerBound();
        if (lowerBound.y < Board::bounds.BOTTOM)
        {
            y.begin++;
            y.end++;
        }
        gravityTimer -= currentInterval;
    }
}

void Tetromino::rotate(const std::vector<Coords>& boardBaseline, bool clockwise)
{
    if (clockwise) rotationIndex = (rotationIndex + 1) % 4;
    else if (!clockwise) rotationIndex = (rotationIndex - 1 + 4) % 4;

    auto& pieceRotations        = rotations.at(shapeType);
    auto& currentRotationCoords = pieceRotations[rotationIndex];

    std::array<std::array<std::string, 4>, 4> grid{{
        {" ", " ", " ", " "},
        {" ", " ", " ", " "},
        {" ", " ", " ", " "},
        {" ", " ", " ", " "},
    }};

    for (const auto& [offsetX, offsetY] : currentRotationCoords)
    {
        int row        = 1 - offsetY; // invert y-axis for correct rotation
        int col        = offsetX + 1; // shift x-axis to fit in 3x3 grid
        grid[row][col] = blocks[shapeType];
    }

    matrix    = grid;
    Axis refX = {x.begin,
                 static_cast<std::int16_t>(x.begin + matrix[0].size() - 1)};
    Axis refY = {y.begin,
                 static_cast<std::int16_t>(y.begin + matrix.size() - 1)};

    wallKick(refX, refY);
}

void Tetromino::moveLeft(std::vector<std::vector<std::string>> grid)
{
    Coords lowerBound = getLowerBound();
    if (lowerBound.x <= Board::bounds.LEFT) return;

    // Check if any block of the piece would collide with locked pieces
    for (auto row = y.begin; row <= y.end; ++row)
        for (auto col = x.begin; col <= x.end; ++col)
            if (matrix[row - y.begin][col - x.begin] != " ")
                if (grid[row][col - 1] != " ") return;

    x.begin--;
    x.end--;
}

void Tetromino::moveRight(std::vector<std::vector<std::string>> grid)
{
    Coords upperBound = getUpperBound();
    if (upperBound.x >= Board::bounds.RIGHT) return;

    // Check if any block of the piece would collide with locked pieces
    for (auto row = y.begin; row <= y.end; ++row)
        for (auto col = x.begin; col <= x.end; ++col)
            if (matrix[row - y.begin][col - x.begin] != " ")
                if (grid[row][col + 1] != " ") return;

    x.begin++;
    x.end++;
}

void Tetromino::hardDrop(const std::vector<Coords>& boardBaseline)
{
    if (boardBaseline.empty()) return;

    std::int16_t highestY = boardBaseline[0].y;
    for (const auto& coord : boardBaseline)
    {
        if (coord.y < highestY) highestY = coord.y;
    }

    std::int16_t occupiedBottom = 0;
    for (std::int16_t row = 0; row < static_cast<std::int16_t>(matrix.size());
         ++row)
    {
        for (std::int16_t col = 0;
             col < static_cast<std::int16_t>(matrix[row].size()); ++col)
        {
            if (matrix[row][col] != " ") occupiedBottom = row;
        }
    }

    y.begin = static_cast<std::int16_t>(highestY - occupiedBottom);
    y.end   = static_cast<std::int16_t>(y.begin + matrix.size() - 1);
}

void Tetromino::softDrop()
{
    speed = 15.0f;
}

void Tetromino::wallKick(Axis& refX, Axis& refY)
{
    if (refX.begin < Board::bounds.LEFT)
    {
        auto delta = static_cast<std::int16_t>(Board::bounds.LEFT - refX.begin);
        refX.begin += delta;
        refX.end   += delta;
    }
    else if (refX.end > Board::bounds.RIGHT)
    {
        auto delta  = static_cast<std::int16_t>(refX.end - Board::bounds.RIGHT);
        refX.begin -= delta;
        refX.end   -= delta;
    }

    x = refX;
    y = refY;
}

Coords Tetromino::getLowerBound()
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

Coords Tetromino::getUpperBound()
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

void Tetromino::calculateBaseline()
{
    baseline.clear();

    for (auto col = x.begin; col <= x.end; ++col)
    {
        for (auto row = y.end; row >= y.begin; --row)
        {
            if (matrix[row - y.begin][col - x.begin] != " ")
            {
                baseline.push_back({col, row});
                break;
            }
        }
    }
}
