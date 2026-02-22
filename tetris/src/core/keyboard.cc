#include "core/debugger.h"
#include "core/keyboard.h"

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#include <windows.h>

ansi::Key Keyboard::poll()
{
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    // Terminal emulators like Mintty (MINGW64/Git Bash) connect stdin through
    // a pipe rather than a real Windows Console. In that case _kbhit() always
    // returns 0 because it only polls the Console input queue. We detect this
    // and fall back to PeekNamedPipe + ANSI escape-sequence parsing, which is
    // the same protocol Mintty uses for arrow/function keys.
    if (GetFileType(hIn) == FILE_TYPE_PIPE)
    {
        DWORD bytesAvail = 0;
        if (!PeekNamedPipe(hIn, NULL, 0, NULL, &bytesAvail, NULL) ||
            bytesAvail == 0)
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

        char ch;
        DWORD bytesRead;
        if (!ReadFile(hIn, &ch, 1, &bytesRead, NULL) || bytesRead == 0)
            return ansi::Key::NONE;

        dbg::log("Read char (pipe): " + std::to_string(static_cast<int>(ch)));

        buffer += ch;

        // Arrow keys (Mintty sends standard ANSI sequences)
        if (buffer == "\x1b[A") { buffer.clear(); return ansi::Key::UP; }
        if (buffer == "\x1b[B") { buffer.clear(); return ansi::Key::DOWN; }
        if (buffer == "\x1b[C") { buffer.clear(); return ansi::Key::RIGHT; }
        if (buffer == "\x1b[D") { buffer.clear(); return ansi::Key::LEFT; }
        if (buffer == " ")      { buffer.clear(); return ansi::Key::SPACEBAR; }
        if (buffer == "z" || buffer == "Z") { buffer.clear(); return ansi::Key::Z; }
        if (buffer == "x" || buffer == "X") { buffer.clear(); return ansi::Key::X; }
        if (buffer == "\x1b" || buffer == "\x1b[") return ansi::Key::NONE;
        if (buffer.size() > 3) buffer.clear();

        return ansi::Key::NONE;
    }

    // Real Windows Console path: use _kbhit() / _getch()
    if (!_kbhit()) return ansi::Key::NONE;

    int ch = _getch();

    dbg::log("Read char: " + std::to_string(ch));

    // Arrow keys and other extended keys are prefixed with 0 or 0xe0
    if (ch == 0 || ch == 0xe0)
    {
        // Both bytes of an extended key sequence are always queued together;
        // read the second byte directly without an additional _kbhit() check.
        int ext = _getch();
        switch (ext)
        {
            case 0x48: return ansi::Key::UP;    // Up arrow
            case 0x50: return ansi::Key::DOWN;  // Down arrow
            case 0x4D: return ansi::Key::RIGHT; // Right arrow
            case 0x4B: return ansi::Key::LEFT;  // Left arrow
        }
        return ansi::Key::NONE;
    }

    switch (ch)
    {
        case ' ':             return ansi::Key::SPACEBAR;
        case 'z': case 'Z':   return ansi::Key::Z;
        case 'x': case 'X':   return ansi::Key::X;
        case 0x1b:            return ansi::Key::ESC;
    }

    return ansi::Key::NONE;
}

#else
#include <errno.h>
#include <unistd.h>

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
#endif
