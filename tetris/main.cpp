#include <iostream>
#include <cstdlib>
#include <array>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>

using clock_type = std::chrono::steady_clock;
using seconds_f  = std::chrono::duration<float>;

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
//    raw.c_oflag &= ~(OPOST);                // output modes

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void setNonBlocking()
{
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

enum Key : std::int8_t {
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
    if (read(STDIN_FILENO, &input, 1) != 1) {
        return true;
    }

    if (input == '\x1B') {
        std::array<char, 2> seq;

        if (read(STDIN_FILENO, seq.data(), 1) != 1) { return false; }
        if (seq[0] != '[') { return false; }

        if (read(STDIN_FILENO, &seq[1], 1) != 1) { return false; }

        switch (seq[1]) {
            case 'A': outKey = UP;    return true;
            case 'B': outKey = DOWN;  return true;
            case 'C': outKey = RIGHT; return true;
            case 'D': outKey = LEFT;  return true;
            default:  return false;
        }
    }

    if (input == ' ') { outKey = SPACEBAR; }
    else if (input == 'c' || input == 'C') { outKey = C_KEY; }
    else if (input == 'z' || input == 'Z') { outKey = Z_KEY; }
    else if (input == 'x' || input == 'X') { outKey = X_KEY; }

    return true;
}


// T, S, Z, L, J, O, I

const std::array<std::array<std::string, 3>, 3> ShapeL{{
    { " ", " ", "■" },
    { "■", "■", "■" },
    { " ", " ", " " },
}};

struct Axis {
    int begin;
    int end;
};

constexpr short TARGET_FPS   = 30;
constexpr auto  FRAME_TIME   = std::chrono::duration<double>(1.0 / TARGET_FPS);
constexpr short BOARD_WIDTH  = 10;
constexpr short BOARD_HEIGHT = 22; // 20 + 2 (2 hidden rows)

std::array<std::array<std::string, BOARD_WIDTH>, BOARD_HEIGHT> board{{
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "}, // hidden rows

    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "},
    {" "," "," "," "," "," "," "," "," "," "}
}};

bool running = true;
auto previous = clock_type::now();

const short xBegin = static_cast<short>(
    std::floor( ( BOARD_WIDTH - 1.0 ) / 2.0) -
    std::floor( ( ShapeL[0].size() - 1.0 ) / 2.0 )
);
const short xEnd = static_cast<short>(xBegin + ShapeL[0].size() - 1);
Axis xAxis = {xBegin, xEnd};
Axis yAxis = {0, 1};

Key key;

constexpr const double INITIAL_GRAVITY_INTERVAL = 0.7; // seconds
constexpr const double INITIAL_GRAVITY_FACTOR   = 1.0; // seconds
constexpr const double BASE_SOFT_DROP_GRAVITY_FACTOR   = 0.3; // seconds

double gravityInterval = INITIAL_GRAVITY_INTERVAL;
double gravityFactor   = INITIAL_GRAVITY_FACTOR;
double gravityTimer    = 0.0;

short currentRotation = 0;

enum Shape : std::uint8_t {
    L = 0,
    T,
    J,
    S,
    Z,
    O,
    I
};

// for T, L, J, S, Z

using array_of_coords = std::array<std::array<std::array<short, 2>, 4>, 4>;
const std::unordered_map<Shape, array_of_coords> rotations{
    {
        Shape::L,
        {{
            {{ {-1, 0}, {0, 0}, {1,  0}, {1,   1} }},
            {{ {0,  1}, {0, 0}, {0, -1}, {1,  -1} }},
            {{ {1,  0}, {0, 0}, {-1, 0}, {-1, -1} }},
            {{ {0, -1}, {0, 0}, {0,  1}, {1,  -1} }}
        }}
    }
};

void handleKey(Key &key)
{
    switch (key)
    {
        case Key::LEFT:
            if (xAxis.begin <= 0) { break; }
            xAxis.begin--;
            xAxis.end--;
            break;
        case Key::RIGHT:
            if (xAxis.end >= BOARD_WIDTH - 1) { break; }
            xAxis.begin++;
            xAxis.end++;
            break;
        case Key::SPACEBAR:
            // hard drop
            yAxis.begin = BOARD_HEIGHT - 2;
            yAxis.end = BOARD_HEIGHT - 1;
            break;
        case Key::C_KEY:
            // hold
            break;
        case Key::X_KEY:
        case Key::UP:
            // clockwise rotation
            break;
        case Key::Z_KEY:
            // counter-clockwise rotation
            break;
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
    if (!readKey(key)) {
        running = false;
    }

    handleKey(key);

    double currentInterval = gravityInterval * gravityFactor;

    gravityTimer += delta;

    while (gravityTimer >= currentInterval) {
        if (yAxis.end >= BOARD_HEIGHT - 1) break;

        yAxis.begin++;
        yAxis.end++;
        gravityTimer -= currentInterval;
    }
}

void render()
{
    homeCursor();

    std::cout << "\n\n";
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        if (row == 0 || row == 1) {
            std::cout << " ";
        } else {
            std::cout << "│";
        }
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            std::string cell = board[row][col];

            if (row   >= yAxis.begin && row   <= yAxis.end &&
                col >= xAxis.begin && col <= xAxis.end
            ) {
                int yAxis = row - yAxis.begin;
                int xAxis = col - xAxis.begin;
                cell = ShapeL[yAxis][xAxis];
            }

            std::cout << cell;
        }
        if (row == 0 || row == 1) {
            std::cout << "\n";
        } else {
            std::cout << "│\n";
        }
    }
    std::cout << "╰——————————╯" << '\n';
}

auto main(int argc, const char * argv[]) -> int
{
    std::cout.setf(std::ios::unitbuf);

    hideCursor();
    enableRawMode();
    setNonBlocking();

    clear();

    while (running) {
        auto frame_start = clock_type::now();

        seconds_f delta = frame_start - previous;
        previous = frame_start;

        update(delta.count());
        render();

        auto frame_end = clock_type::now();
        auto work_time = frame_end - frame_start;

        if (work_time < FRAME_TIME) {
            std::this_thread::sleep_for(FRAME_TIME - work_time);
        }
    }

    restoreCursor();

    return 0;
}
