#include "debugger.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

using clock_type = std::chrono::steady_clock;
using seconds_f  = std::chrono::duration<float>;

AsyncLogger logger;

struct Axis
{
    short begin;
    short end;
};

struct Coords
{
    short x;
    short y;
};

enum Shape : std::uint8_t
{
    L = 0,
    T,
    J,
    S,
    Z,
    O,
    I
};

inline void clear()
{
    std::cout << "\033[H\033[2J";
}

inline void hideCursor()
{
    std::cout << "\033[?25l";
}

inline void restoreCursor()
{
    std::cout << "\033[?25h";
}

inline void homeCursor()
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

enum Key : std::int8_t
{
    NONE = -1,
    UP,
    DOWN,
    RIGHT,
    LEFT,
    SPACEBAR,
    ESC,
    Z_KEY,
    X_KEY,
    C_KEY
};

auto readKey(Key& outKey) -> bool
{
    outKey = Key::NONE;

    char input;
    if (read(STDIN_FILENO, &input, 1) != 1) { return true; }

    if (input == '\x1B')
    {
        std::array<char, 2> seq;

        if (read(STDIN_FILENO, seq.data(), 1) != 1) { return false; }
        if (seq[0] != '[') { return false; }

        if (read(STDIN_FILENO, &seq[1], 1) != 1) { return false; }

        switch (seq[1])
        {
            case 'A':
                outKey = UP;
                return true;
            case 'B':
                outKey = DOWN;
                return true;
            case 'C':
                outKey = RIGHT;
                return true;
            case 'D':
                outKey = LEFT;
                return true;
            default:
                return false;
        }
    }

    if (input == ' ') { outKey = SPACEBAR; }
    else if (input == 'c' || input == 'C') { outKey = C_KEY; }
    else if (input == 'z' || input == 'Z') { outKey = Z_KEY; }
    else if (input == 'x' || input == 'X') { outKey = X_KEY; }

    return true;
}

// T, S, Z, L, J, O, I

std::vector<Coords> baseline;

std::array<std::array<std::string, 3>, 3> currentShape{{
    {" ", " ", "■"},
    {"■", "■", "■"},
    {" ", " ", " "},
}};

constexpr short TARGET_FPS   = 30;
constexpr auto FRAME_TIME    = std::chrono::duration<double>(1.0 / TARGET_FPS);
constexpr short BOARD_WIDTH  = 10;
constexpr short BOARD_HEIGHT = 22; // 20 + 2 (2 hidden rows)

std::array<std::array<std::string, BOARD_WIDTH>, BOARD_HEIGHT> board{
    {{" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "}, // hidden rows

     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "}}};

bool running  = true;
auto previous = clock_type::now();

Axis x = {0, 0};
Axis y = {0, 0};

Key key;

constexpr const double INITIAL_GRAVITY_INTERVAL      = 0.7; // seconds
constexpr const double INITIAL_GRAVITY_FACTOR        = 1.0; // seconds
constexpr const double BASE_SOFT_DROP_GRAVITY_FACTOR = 0.3; // seconds

double gravityInterval = INITIAL_GRAVITY_INTERVAL;
double gravityFactor   = INITIAL_GRAVITY_FACTOR;
double gravityTimer    = 0.0;

short currentRotation = 0;

using array_of_coords = std::array<std::array<std::array<short, 2>, 4>, 4>;
const std::unordered_map<Shape, array_of_coords> rotations{
    {Shape::L,
     {{{{{-1, 0}, {0, 0}, {1, 0}, {1, 1}}},
       {{{0, 1}, {0, 0}, {0, -1}, {1, -1}}},
       {{{1, 0}, {0, 0}, {-1, 0}, {-1, -1}}},
       {{{0, -1}, {0, 0}, {0, 1}, {-1, 1}}}}}}};

/**
 * Recalculates the baseline position based on the current shape and the board
 * state.
 *
 * The baseline is a set of coodinates [(x,y), ...] in relation to the board and
 * the current shape. It represents the lowest point of the current shape + the
 * distance to the nearest occupied cell on the board below it.
 *
 * It is used to determine when the shape should lock into place on the board.
 *
 */
void recalculateBaseline()
{
    baseline.clear();

    bool done = false;
    for (short row = y.end; row >= y.begin; --row)
    {
        for (short col = x.begin; col <= x.end; ++col)
        {
            if (currentShape[row - y.begin][col - x.begin] == "■")
            {
                short r = row + 1;
                baseline.push_back({col, r});
                if (col >= x.end) done = true;
            }
        }
        if (done) break;
    }

    for (auto& coord : baseline)
    {
        while (coord.y < BOARD_HEIGHT && board[coord.y][coord.x] == " ")
            coord.y++;
        coord.y--; // step back to the last empty cell
    }

    for (int i = 0; i < baseline.size(); ++i)
    {
        logger.log("Baseline coord " + std::to_string(i) + ": (" +
                   std::to_string(baseline[i].x) + ", " +
                   std::to_string(baseline[i].y) + ")");
    }
}

void clearLines()
{
    for (short row = BOARD_HEIGHT - 1; row >= 0; --row)
    {
        bool fullLine = true;
        for (short col = 0; col < BOARD_WIDTH; ++col)
        {
            if (board[row][col] == " ")
            {
                fullLine = false;
                break;
            }
        }

        if (fullLine)
        {
            // Move all rows above down by one
            for (short r = row; r > 0; --r)
            {
                board[r] = board[r - 1];
            }
            // Clear the top row
            board[0] = {" ", " ", " ", " ", " ", " ", " ", " ", " ", " "};
            // Check the same row again since it now contains the above row
            row++;
        }
    }
}

void spawnNewPiece()
{
    currentShape = {{
        {" ", " ", "■"},
        {"■", "■", "■"},
        {" ", " ", " "},
    }};

    x.begin =
        static_cast<short>(std::floor((BOARD_WIDTH - 1.0) / 2.0) -
                           std::floor((currentShape[0].size() - 1.0) / 2.0));
    x.end   = static_cast<short>(x.begin + currentShape[0].size() - 1);
    y.begin = 0;
    y.end   = static_cast<short>(y.begin + currentShape.size() - 1);

    currentRotation = 0;
}

void lockPiece()
{
    if (baseline.empty()) return;

    // Check if any baseline coordinate matches the current piece position
    bool shouldLock = false;
    for (const auto& coords : baseline)
    {
        for (short row = y.end; row >= y.begin; --row)
        {
            for (short col = x.begin; col <= x.end; ++col)
            {
                if (currentShape[row - y.begin][col - x.begin] == "■")
                {
                    if (coords.x == col && coords.y == row)
                    {
                        shouldLock = true;
                        break;
                    }
                }
            }
            if (shouldLock) break;
        }
        if (shouldLock) break;
    }

    // If collision detected, lock the piece and spawn new one
    if (shouldLock)
    {
        for (short row = y.end; row >= y.begin; --row)
        {
            for (short col = x.begin; col <= x.end; ++col)
            {
                if (currentShape[row - y.begin][col - x.begin] == "■")
                {
                    board[row][col] = "■";
                }
            }
        }

        baseline.clear();
        spawnNewPiece();
    }
}

void handleKey(Key& key)
{
    switch (key)
    {
        case Key::LEFT:
            if (baseline.empty() || baseline[0].x <= 0) break;
            x.begin--;
            x.end--;
            break;
        case Key::RIGHT:
            if (baseline.empty() ||
                baseline[baseline.size() - 1].x >= BOARD_WIDTH - 1)
                break;
            x.begin++;
            x.end++;
            break;
        case Key::SPACEBAR:
        {
            // hard drop
            short highestY = baseline[0].y;
            for (const auto& coord : baseline)
            {
                if (coord.y < highestY) { highestY = coord.y; }
            }
            y.end   = highestY;
            y.begin = static_cast<short>(y.end - currentShape.size() + 1);
            break;
        }
        case Key::C_KEY:
            // hold
            break;
        case Key::X_KEY:
        case Key::UP:
        {
            currentRotation = (currentRotation + 1) % 4;
            // clockwise rotation
            auto& pieceRotations        = rotations.at(Shape::L);
            auto& currentRotationCoords = pieceRotations[currentRotation];

            std::array<std::array<std::string, 3>, 3> grid{{
                {" ", " ", " "},
                {" ", " ", " "},
                {" ", " ", " "},
            }};

            for (const auto& [x, y] : currentRotationCoords)
            {
                int row        = 1 - y; // invert y-axis for correct rotation
                int col        = x + 1; // shift x-axis to fit in 3x3 grid
                grid[row][col] = "■";
            }

            currentShape = grid;
            x            = {x.begin,
                            static_cast<short>(x.begin + currentShape[0].size() - 1)};
            y            = {y.begin,
                            static_cast<short>(y.begin + currentShape.size() - 1)};
            break;
        }
        case Key::Z_KEY:
        {
            currentRotation = (currentRotation - 1 + 4) % 4;
            // clockwise rotation
            auto& pieceRotations        = rotations.at(Shape::L);
            auto& currentRotationCoords = pieceRotations[currentRotation];

            std::array<std::array<std::string, 3>, 3> grid{{
                {" ", " ", " "},
                {" ", " ", " "},
                {" ", " ", " "},
            }};

            for (const auto& [x, y] : currentRotationCoords)
            {
                int row        = 1 - y; // invert y-axis for correct rotation
                int col        = x + 1; // shift x-axis to fit in 3x3 grid
                grid[row][col] = "■";
            }

            currentShape = grid;
            x            = {x.begin,
                            static_cast<short>(x.begin + currentShape[0].size() - 1)};
            y            = {y.begin,
                            static_cast<short>(y.begin + currentShape.size() - 1)};

            break;
        }
        case Key::DOWN:
            // soft drop
            gravityFactor = BASE_SOFT_DROP_GRAVITY_FACTOR;
            break;
        default:
            // reset states
            gravityFactor = INITIAL_GRAVITY_FACTOR;
            break;
    }
}

void update(double delta)
{
    if (!readKey(key)) running = false;

    handleKey(key);

    double currentInterval  = gravityInterval * gravityFactor;
    gravityTimer           += delta;

    while (gravityTimer >= currentInterval)
    {
        y.begin++;
        y.end++;
        gravityTimer -= currentInterval;
    }

    recalculateBaseline();
    lockPiece();
    clearLines();
}

void render()
{
    homeCursor();

    std::cout << "\n\n";
    for (int row = 0; row < BOARD_HEIGHT; ++row)
    {
        // left border
        if (row >= 2) std::cout << "│";
        else std::cout << " ";

        for (int col = 0; col < BOARD_WIDTH; ++col)
        {
            std::string cell = board[row][col];

            if (row >= y.begin && row <= y.end && col >= x.begin &&
                col <= x.end)
            {
                int yAxis = row - y.begin;
                int xAxis = col - x.begin;
                if (currentShape[yAxis][xAxis] == "■") cell = "■";
            }

            std::cout << cell;
        }

        // right border
        if (row >= 2) std::cout << "│\n";
        else std::cout << "\n";
    }
    std::cout << "╰——————————╯" << '\n';
}

auto main(int argc, const char* argv[]) -> int
{
    logger.start("debug.log");

    std::cout.setf(std::ios::unitbuf);

    hideCursor();
    enableRawMode();
    setNonBlocking();

    clear();

    spawnNewPiece();

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

    restoreCursor();
    disableRawMode();

    logger.stop();

    return 0;
}
