#include <iostream>
#include <cstdlib>
#include <array>
#include <chrono>
#include <thread>

using clock_type = std::chrono::steady_clock;
using seconds_f  = std::chrono::duration<float>;

// Tetris Rules
// 2 Hidden Rows
// Table Size 10x20
// Gravity 0.02

inline void clear()
{
    std::cout << "\033[2J";
}

inline void home_cursor()
{
    std::cout << "\033[H";
}

// T, S, Z, L, J, O, I

const std::array<std::array<std::string, 3>, 2> L{{
    { " ", " ", "■" },
    { "■", "■", "■" },
}};

//const char tile = u8'■';

constexpr short TARGET_FPS   = 60;
constexpr auto  FRAME_TIME   = std::chrono::duration<double>(1.0 / TARGET_FPS);
constexpr short BOARD_WIDTH  = 10;
constexpr short BOARD_HEIGHT = 22; // 20 + 2 (2 hidden rows)

std::array<std::array<std::string, BOARD_WIDTH>, BOARD_HEIGHT> board{{
    {" "," "," ","3","4","5"," "," "," "," "},
    {" "," "," ","6","7","8"," "," "," "," "}, // hidden rows
    
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

void update(double dt)
{
    
}

struct Axis {
    int begin;
    int end;
};

void render()
{
    home_cursor();
    
    const short x1 = std::floor(( BOARD_WIDTH - 1 ) / 2) - std::floor(( L[0].size() - 1 ) / 2);
    const short x2 = x1 + L[0].size() - 1;
    Axis x = {x1, x2};
    Axis y = {0, 1};
//    
//    std::cout << x.begin << "->" << x.end << '\n';
//    std::cout << y.begin << "->" << y.end << '\n';
    
    int k = 0;
    for (int i = y.begin; i <= y.end; ++i) {
        for (int j = x.begin; j <= x.end; ++j, ++k) {
            board[i][j] = L[i][k];
        }
        k = 0;
    }
    
    for (const auto& row : board) {
        std::cout << "│";
        for (const auto& cell : row) {
            std::cout << cell;
        }
        std::cout << "│\n";
    }
    std::cout << "╰──────────╯" << '\n';
}

auto main(int argc, const char * argv[]) -> int
{
    std::cout.setf(std::ios::unitbuf);
    
    clear();

    bool running = true;
    auto previous = clock_type::now();

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
    
    return 0;
}
