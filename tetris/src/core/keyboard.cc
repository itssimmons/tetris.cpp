#include <array>
#include <unistd.h>

#include "core/ansi.h"
#include "core/keyboard.h"

void Keyboard::listen(std::function<std::function<void()>(ansi::Key)> callback)
{
    ansi::Key currentKey = ansi::Key::NONE;
    char input           = 0;

    const ssize_t n = read(STDIN_FILENO, &input, 1);

    // Nothing available (EAGAIN) or EOF: treat as key release
    if (n <= 0)
    {
        if (releaseCallback && holding) { releaseCallback(); }
        holding = false;
        return;
    }

    // Check if this is an escape sequence (arrow keys, etc.)
    if (input == '\x1b')
    {
        std::array<char, 2> seq{};
        if (read(STDIN_FILENO, &seq[0], 1) == 1 && seq[0] == '[')
        {
            if (read(STDIN_FILENO, &seq[1], 1) == 1)
            {
                currentKey = ansi::typeofKey[seq[1]];
            }
        }
        else
        {
            // If no sequence follows, it's just the ESC key
            currentKey = ansi::typeofKey[input];
        }
    }
    else
    {
        currentKey = ansi::typeofKey[input];
    }

    if (currentKey != ansi::Key::NONE)
    {
        holding         = true;
        releaseCallback = callback(currentKey);
    }
    else
    {
        if (releaseCallback && holding) { releaseCallback(); }
        holding = false;
    }
};
