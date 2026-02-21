#include <errno.h>
#include <unistd.h>

#include "core/debugger.h"
#include "core/keyboard.h"

ansi::Key Keyboard::poll()
{
    char ch;
    ssize_t n = read(STDIN_FILENO, &ch, 1);

    dbg::log("Read char: " + std::to_string(ch) + " (n=" + std::to_string(n) +
             ")");

    if (n == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // If we have a lone ESC sitting in the buffer and no more input,
            // then it's actually an ESC keypress
            if (buffer == "\x1b")
            {
                buffer.clear();
                return ansi::Key::ESC;
            }
            return ansi::Key::NONE;
        }

        return ansi::Key::NONE;
    }

    if (n == 0)
    {
        if (buffer == "\x1b")
        {
            buffer.clear();
            return ansi::Key::ESC;
        }
        return ansi::Key::NONE;
    }

    buffer += ch;

    // Arrow keys
    if (buffer == "\x1b[A")
    {
        buffer.clear();
        return ansi::Key::UP;
    }
    if (buffer == "\x1b[B")
    {
        buffer.clear();
        return ansi::Key::DOWN;
    }
    if (buffer == "\x1b[C")
    {
        buffer.clear();
        return ansi::Key::RIGHT;
    }
    if (buffer == "\x1b[D")
    {
        buffer.clear();
        return ansi::Key::LEFT;
    }
    if (buffer == " ") // Spacebar
    {
        buffer.clear();
        return ansi::Key::SPACEBAR;
    }
    if (buffer == "z" || buffer == "Z") // Z key
    {
        buffer.clear();
        return ansi::Key::Z;
    }
    if (buffer == "x" || buffer == "X") // X key
    {
        buffer.clear();
        return ansi::Key::X;
    }
    // ESC partial sequence - wait for more characters
    if (buffer == "\x1b" || buffer == "\x1b[") return ansi::Key::NONE;

    // If buffer grows too long, reset
    if (buffer.size() > 3) buffer.clear();

    return ansi::Key::NONE;
}
