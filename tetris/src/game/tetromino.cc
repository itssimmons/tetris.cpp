#include <cstdint>
#include <unordered_map>

#include "core/rng.h"
#include "game/board.h"
#include "game/tetromino.h"

std::unordered_map<Shape, matrix_t> shapes{
    {Shape::L,
     {{{" ", " ", "■", " "},
       {"■", "■", "■", " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::T,
     {{{" ", "■", " ", " "},
       {"■", "■", "■", " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::J,
     {{{"■", " ", " ", " "},
       {"■", "■", "■", " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::S,
     {{{" ", "■", "■", " "},
       {"■", "■", " ", " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::Z,
     {{{"■", "■", " ", " "},
       {" ", "■", "■", " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::O,
     {{{"■", "■", " ", " "},
       {"■", "■", " ", " "},
       {" ", " ", " ", " "},
       {" ", " ", " ", " "}}}},
    {Shape::I,
     {{{" ", " ", " ", " "},
       {"■", "■", "■", "■"},
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

void Tetromino::fallLoop(double& dt, double& gravityInterval,
                         double& gravityTimer)
{
    double currentInterval  = gravityInterval / speed;
    gravityTimer           += dt;

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
        grid[row][col] = "■";
    }

    matrix = grid;
    x = {x.begin, static_cast<std::int16_t>(x.begin + matrix[0].size() - 1)};
    y = {y.begin, static_cast<std::int16_t>(y.begin + matrix.size() - 1)};

    wallKick();
}

void Tetromino::moveLeft(const std::vector<Coords>& baseline)
{
    if (baseline.empty() || baseline[0].x <= Board::bounds.LEFT) return;
    x.begin--;
    x.end--;
}

void Tetromino::moveRight(const std::vector<Coords>& baseline)
{
    if (baseline.empty() ||
        baseline[baseline.size() - 1].x >= Board::bounds.RIGHT)
        return;
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
    speed += 5.0f;
}

void Tetromino::wallKick()
{
    if (x.begin < 0)
    {
        x.begin++;
        x.end++;
    }
    else if (x.end >= Board::bounds.RIGHT)
    {
        x.begin--;
        x.end--;
    }
}
