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

const std::unordered_map<Shape, matrix_trot> rotations{
    {Shape::L,
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
    speed         = 1.0f;
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

void Tetromino::fallLoop(double& dt, const std::vector<Coords>& baseline)
{
    double currentInterval  = gravityInterval;
    gravityTimer           += dt * speed;

    while (gravityTimer >= currentInterval)
    {
        y.begin++;
        y.end++;
        gravityTimer -= currentInterval;
    }
}

void Tetromino::rotate(const std::vector<Coords>& baseline, bool clockwise)
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

void Tetromino::moveLeft(const std::vector<Coords>& baseline)
{
    Coords lowerBound = getLowerBound(baseline);
    if (baseline.empty() || lowerBound.x <= Board::bounds.LEFT) return;
    x.begin--;
    x.end--;
}

void Tetromino::moveRight(const std::vector<Coords>& baseline)
{
    Coords upperBound = getUpperBound(baseline);
    if (baseline.empty() || upperBound.x >= Board::bounds.RIGHT) return;
    x.begin++;
    x.end++;
}

void Tetromino::hardDrop(const std::vector<Coords>& baseline)
{
    std::int16_t highestY = baseline[0].y;
    for (const auto& coord : baseline)
    {
        if (coord.y < highestY) highestY = coord.y;
    }
    y.end   = highestY;
    y.begin = static_cast<std::int16_t>(y.end - matrix.size() + 1);
}

void Tetromino::softDrop()
{
    speed = 10.0f;
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

Coords Tetromino::getLowerBound(const std::vector<Coords>& baseline)
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

Coords Tetromino::getUpperBound(const std::vector<Coords>& baseline)
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
