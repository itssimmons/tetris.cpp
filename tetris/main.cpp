#include <iostream>
#include <cstdlib>
#include <array>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

using clock_type = std::chrono::steady_clock;
using seconds_f  = std::chrono::duration<float>;

// Tetris Rules
// 2 Hidden Rows
// Table Size 10x20
// Gravity 0.02

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

enum Key { NONE = -1, UP, DOWN, RIGHT, LEFT, SPACEBAR, ESC, C };

bool readKey(Key& outKey)
{
    outKey = Key::NONE;

    char c;
    if (read(STDIN_FILENO, &c, 1) != 1)
        return true;

    if (c == '\x1B') {
        char seq[2];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return false;
        if (seq[0] != '[') return false;

        if (read(STDIN_FILENO, &seq[1], 1) != 1) return false;

        switch (seq[1]) {
            case 'A': outKey = UP;    return true;
            case 'B': outKey = DOWN;  return true;
            case 'C': outKey = RIGHT; return true;
            case 'D': outKey = LEFT;  return true;
            default:  return false;
        }
    }

    if (c == ' ') outKey = SPACEBAR;
    else if (c == 'c' || c == 'C') outKey = C;

    return true;
}


// T, S, Z, L, J, O, I

const std::array<std::array<std::string, 3>, 2> L{{
    { " ", " ", "■" },
    { "■", "■", "■" },
}};

struct Axis {
    int begin;
    int end;
};

constexpr short TARGET_FPS   = 60;
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

const short x1 = std::floor(( BOARD_WIDTH - 1 ) / 2) - std::floor(( L[0].size() - 1 ) / 2);
const short x2 = x1 + L[0].size() - 1;
Axis x = {x1, x2};
Axis y = {0, 1};

Key key;

int baseline = y.end;

double gravityTimer = 0.0;
double gravityInterval = 0.8;

void handleKey(Key &key)
{
    switch (key) {
        case Key::LEFT:
            if (x.begin <= 0) break;
            x.begin--;
            x.end--;
            break;
        case Key::RIGHT:
            if (x.end >= BOARD_WIDTH - 1) break;
            x.begin++;
            x.end++;
            break;
        default:
            break;
    }
}

void update(double dt)
{
    if (!readKey(key))
        running = false;
    
    handleKey(key);
    
    gravityTimer += dt;
    while (gravityTimer >= gravityInterval) {
        if (y.end >= BOARD_HEIGHT-1) break;
        y.begin++;
        y.end++;
        gravityTimer -= gravityInterval;
    }
}

void render()
{
    homeCursor();
    
    std::cout << "\n\n";
    for (int r = 0; r < BOARD_HEIGHT; ++r) {
        std::cout << "│";
        for (int c = 0; c < BOARD_WIDTH; ++c) {
            std::string cell = board[r][c];
            
            if (r >= y.begin && r <= y.end &&
                c>= x.begin && c <= x.end
            ) {
                int yAxis = r - y.begin;
                int xAxis = c - x.begin;
                cell = L[yAxis][xAxis];
            }

            std::cout << cell;
        }
        std::cout << "│\n";
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
        
        if (work_time < FRAME_TIME)
            std::this_thread::sleep_for(FRAME_TIME - work_time);
    }
    
    restoreCursor();
    
    return 0;
}
