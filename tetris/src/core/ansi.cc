#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>

#include "core/ansi.h"

namespace ansi
{
void clearScreen()
{
    std::cout << "\033[H\033[2J";
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

termios orig_termios;

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    termios raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON | ISIG); // local modes
    raw.c_iflag &= ~(IXON | ICRNL);         // input modes
    // raw.c_oflag &= ~(OPOST);             // output modes

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void setNonBlocking()
{
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}
} // namespace ansi
