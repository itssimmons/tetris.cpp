#pragma once

#ifndef TERMINAL_H
#define TERMINAL_H

#include <cstdint>

namespace terminal
{
inline std::uint16_t WIDTH  = 10;
inline std::uint16_t HEIGHT = 20;

void init();
} // namespace terminal

#endif // TERMINAL_H
