#pragma once

#ifndef COORDS_H
#define COORDS_H

#include <cstdint>

struct Axis
{
    std::int16_t begin;
    std::int16_t end;
};

struct Coords
{
    std::int16_t x;
    std::int16_t y;
};

#endif // COORDS_H
