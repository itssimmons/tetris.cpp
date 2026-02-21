#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>

#include "core/ansi.h"

namespace ansi
{
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
} // namespace ansi
