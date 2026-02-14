#include <chrono>
#include <iostream>
#include <thread>

#include "core/ansi.h"
#include "core/engine.h"
#include "core/keyboard.h"
#include "game/board.h"
#include "game/tetromino.h"

double gravityInterval = INITIAL_GRAVITY_INTERVAL;
double gravityFactor   = INITIAL_GRAVITY_FACTOR;

Tetromino piece;
Board board;
Keyboard keyboard;

void Engine::loop()
{
    while (running)
    {
        auto frame_start = clock_type::now();

        seconds_f delta = frame_start - previous;
        previous        = frame_start;

        update(delta.count());
        render();

        auto frame_end = clock_type::now();
        auto work_time = frame_end - frame_start;

        if (work_time < FRAME_TIME)
            std::this_thread::sleep_for(FRAME_TIME - work_time);
    }
}

void Engine::render()
{
    ansi::homeCursor();
    board.render(piece);
}

void Engine::update(double dt)
{
    keyboard.listen(
        [&](ansi::Key key)
        {
            if (key == ansi::Key::LEFT) piece.moveLeft(board.baseline);
            else if (key == ansi::Key::RIGHT) piece.moveRight(board.baseline);
            else if (key == ansi::Key::SPACEBAR) piece.hardDrop(board.baseline);
            else if (key == ansi::Key::DOWN) piece.softDrop();
            else if (key == ansi::Key::Z)
                piece.rotate(board.baseline, false); // counter-clockwise
            else if (key == ansi::Key::X || key == ansi::Key::UP)
                piece.rotate(board.baseline, true); // clockwise
            else if (key == ansi::Key::ESC) running = false;

            return []()
            {
                // release key
                piece.speed = 1.0f; // reset speed on key release
            };
        });

    piece.fallLoop(dt, gravityInterval, gravityFactor);

    board.calculateBaseline(piece);
    board.lockPiece(piece);
    board.clearLines();
    board.gameOver(running);
}

int Engine::run()
{
    Engine engine;

    std::cout.setf(std::ios::unitbuf);

    ansi::hideCursor();
    ansi::enableRawMode();
    ansi::setNonBlocking();
    ansi::clearScreen();

    piece.spawn();

    engine.loop();

    ansi::restoreCursor();

    return 0;
}
