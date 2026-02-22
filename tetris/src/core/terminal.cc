#include "core/terminal.h"

#include <cstdint>
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace terminal
{
void init()
{
#if defined(_WIN32) || defined(_WIN64)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    bool detected = false;

    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        WIDTH  = static_cast<std::uint16_t>(csbi.srWindow.Right -
                                            csbi.srWindow.Left + 1);
        HEIGHT = static_cast<std::uint16_t>(csbi.srWindow.Bottom -
                                            csbi.srWindow.Top + 1);
        detected = true;
    }

    if (!detected)
    {
        // Fallback for terminal emulators (e.g. Mintty/MINGW64) where
        // stdout is a pipe and GetConsoleScreenBufferInfo is unavailable.
        // These environments typically export COLUMNS and LINES.
        const char* cols = std::getenv("COLUMNS");
        const char* rows = std::getenv("LINES");
        if (cols && *cols)
        {
            long w = std::strtol(cols, nullptr, 10);
            if (w > 0 && w <= 0xFFFF)
                WIDTH = static_cast<std::uint16_t>(w);
        }
        if (rows && *rows)
        {
            long h = std::strtol(rows, nullptr, 10);
            if (h > 0 && h <= 0xFFFF)
                HEIGHT = static_cast<std::uint16_t>(h);
        }
    }
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    WIDTH  = static_cast<std::uint16_t>(w.ws_col);
    HEIGHT = static_cast<std::uint16_t>(w.ws_row);
#endif
}
} // namespace terminal
