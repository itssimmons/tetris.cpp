#pragma once

#ifndef ANSI_H
#define ANSI_H

#include <cstdint>
#include <unordered_map>

namespace ansi
{
void clearScreen();

void hideCursor();

void restoreCursor();

void homeCursor();

void disableRawMode();

void enableRawMode();

void setNonBlocking();

enum class Key : std::int8_t
{
    NONE = -1,
    UP,
    DOWN,
    RIGHT,
    LEFT,
    SPACEBAR,
    ESC,
    Z,
    X
};

struct KeyHash
{
    std::size_t operator()(Key key) const
    {
        return static_cast<std::size_t>(key);
    }
};

inline std::unordered_map<char, Key> typeofKey{
    {'\0', Key::NONE},  {'A', Key::UP},   {'B', Key::DOWN},
    {'C', Key::RIGHT},  {'D', Key::LEFT}, {' ', Key::SPACEBAR},
    {'\033', Key::ESC}, {'z', Key::Z},    {'x', Key::X},
};
}; // namespace ansi

#endif // ANSI_H
