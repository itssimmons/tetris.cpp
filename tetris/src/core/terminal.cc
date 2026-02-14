#include "core/terminal.h"

#include <cstdint>

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
    int columns, rows;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows    = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    WIDTH  = static_cast<std::uint16_t>(columns);
    HEIGHT = static_cast<std::uint16_t>(rows);
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    WIDTH  = static_cast<std::uint16_t>(w.ws_col);
    HEIGHT = static_cast<std::uint16_t>(w.ws_row);
#endif
}
} // namespace terminal
