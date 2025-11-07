#pragma once

#include <vector>
#include <utility>

struct Position {
    int x;
    int y;
};

struct KnightBoard {
    std::vector<std::vector<int>> board;
    int size;
    
    KnightBoard(int n) : size(n), board(n, std::vector<int>(n, -1)) {}
};

extern const std::vector<Position> KNIGHT_MOVES;