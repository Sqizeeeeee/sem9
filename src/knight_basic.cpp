#include "knight_basic.h"
#include <iostream>

const std::vector<Position> KNIGHT_MOVES = {
    {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
    {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
};

bool KnightBasic::isValid(int x, int y) const {
    return x >= 0 && x < board.size && 
           y >= 0 && y < board.size &&
           board.board[x][y] == -1;  // Проверяем что клетка не посещена
}

bool KnightBasic::findTour(int x, int y, int step) {
    board.board[x][y] = step;

    if (step == board.size * board.size) {
        return true;
    }

    // Перебрать все возможные ходы
    for (const auto& move : KNIGHT_MOVES) {
        int new_x = x + move.x;
        int new_y = y + move.y;
        
        if (isValid(new_x, new_y)) {
            if (findTour(new_x, new_y, step + 1)) {
                return true;
            }
        }
    }

    // Backtrack - если ни один ход не привел к решению
    board.board[x][y] = -1;
    return false;
}

bool KnightBasic::solve(int start_x, int start_y) {
    return findTour(start_x, start_y, 1);
}

void KnightBasic::printSolution() const {
    std::cout << "Solution found! Tour path:" << std::endl;
    
    // Создаем вектор для хранения пути
    std::vector<Position> path(board.size * board.size);
    
    // Заполняем путь
    for (int i = 0; i < board.size; ++i) {
        for (int j = 0; j < board.size; ++j) {
            int step = board.board[i][j];
            path[step - 1] = {i, j};
        }
    }
    
    // Выводим путь по шагам
    for (int step = 0; step < path.size(); ++step) {
        std::cout << "Step " << (step + 1) << ": (" 
                  << path[step].x << ", " << path[step].y << ")";
        if ((step + 1) % 5 == 0) std::cout << std::endl;
        else std::cout << " -> ";
    }
    std::cout << std::endl;
    
    // И саму доску для наглядности
    std::cout << "\nBoard view:" << std::endl;
    for (int i = 0; i < board.size; ++i) {
        for (int j = 0; j < board.size; ++j) {
            std::cout << board.board[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}