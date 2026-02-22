#include <cstdlib>
#include <iostream>

#include "core/ansi.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace ansi
{

#if defined(_WIN32) || defined(_WIN64)
static DWORD orig_console_mode_in;
static DWORD orig_console_mode_out;

void clearScreen()
{
    std::cout << "\033[H\033[2J";
}

void hideCursor()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void restoreCursor()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void homeCursor()
{
    std::cout << "\033[H";
}

void disableRawMode()
{
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), orig_console_mode_in);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), orig_console_mode_out);
}

void enableRawMode()
{
    HANDLE hIn  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hIn == INVALID_HANDLE_VALUE || hOut == INVALID_HANDLE_VALUE) return;

    if (!GetConsoleMode(hIn, &orig_console_mode_in)) return;
    if (!GetConsoleMode(hOut, &orig_console_mode_out)) return;
    atexit(disableRawMode);

    DWORD inMode = orig_console_mode_in;
    inMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(hIn, inMode);

    // Enable ANSI escape sequences for output (Windows 10+)
    DWORD outMode = orig_console_mode_out;
    outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, outMode);
}

void setNonBlocking()
{
    // Non-blocking input is handled via _kbhit() on Windows
}

#else
static termios orig_termios;

void clearScreen()
{
    /// NOTE: Clearing the screen every frame was causing a lot of flickering.
    /// Instead, we can just move the cursor to the home position and overwrite
    /// the existing content.

    // std::cout << "\033[H\033[2J";
}

void hideCursor()
{
    std::cout << "\033[?25l";
}

void restoreCursor()
{
    std::cout << "\033[?25h";
}

void homeCursor()
{
    std::cout << "\033[H";
}

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    termios raw = orig_termios;

    raw.c_lflag     &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag     &= ~(IXON | ICRNL);
    raw.c_cc[VMIN]   = 0;
    raw.c_cc[VTIME]  = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void setNonBlocking()
{
    /// NOTE: This was causing the midgame freezing, so I'm leaving it out for
    /// now. It seems that enabling raw mode with VMIN=0 and VTIME=0 is
    /// sufficient for non-blocking input.

    // int flags = fcntl(STDIN_FILENO,
    // F_GETFL, 0); fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}
#endif

} // namespace ansi
