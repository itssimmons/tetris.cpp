#include <chrono>
#include <iostream>
#include <thread>

#include "core/ansi.h"
#include "core/debugger.h"
#include "core/engine.h"
#include "core/keyboard.h"
#include "game/board.h"
#include "game/tetromino.h"

Tetromino piece;
Board board;
Keyboard keyboard;

void Engine::loop()
{
    while (running)
    {
        auto frameStart = clock_type::now();

        ansi::Key key = keyboard.poll();

        if (key == ansi::Key::ESC) running = false;

        if (key == ansi::Key::UP) piece.rotate(board.baseline); // clockwise
        if (key == ansi::Key::DOWN) piece.softDrop();
        if (key == ansi::Key::LEFT) piece.moveLeft(board.grid);
        if (key == ansi::Key::RIGHT) piece.moveRight(board.grid);
        if (key == ansi::Key::SPACEBAR) piece.hardDrop(board.baseline);
        if (key == ansi::Key::X) piece.rotate(board.baseline); // clockwise
        if (key == ansi::Key::Z)
            piece.rotate(board.baseline, false); // counter-clockwise

        if (key == ansi::Key::NONE)
        {
            // clean-up
            piece.speed = 3.5f;
        }

        seconds_f delta = frameStart - previous;
        previous        = frameStart;

        update(delta.count());
        render();

        std::cout.flush();

        auto frameEnd = clock_type::now();
        auto workTime = frameEnd - frameStart;

        if (workTime < FRAME_TIME)
            std::this_thread::sleep_for(FRAME_TIME - workTime);
    }
}

void Engine::render()
{
    ansi::homeCursor();
    board.render(piece);
}

void Engine::update(double dt)
{
    // Compute baseline from current position
    board.calculateBaseline(piece);

    piece.fallLoop(dt, board.baseline);

    // Recompute baseline after movement for locking
    board.calculateBaseline(piece);
    board.lockPiece(piece);
    board.clearLines();
    board.gameOver(running);
}

int Engine::run()
{
    Engine engine;

    dbg::start("debug.log");

    std::cout.setf(std::ios::unitbuf);

    ansi::hideCursor();
    ansi::enableRawMode();
    ansi::setNonBlocking();
    // ansi::clearScreen();

    piece.spawn();

    engine.loop();

    ansi::restoreCursor();

    dbg::stop();

    return 0;
}
