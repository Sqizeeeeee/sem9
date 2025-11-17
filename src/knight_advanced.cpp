#include "knight_advanced.h"
#include <iostream>
#include <algorithm>
#include <vector>

bool KnightAdvanced::isValid(int x, int y) const {
    return x >= 0 && x < board.size && y >= 0 && y < board.size && board.board[x][y] == -1;
}

int KnightAdvanced::countAvailableMoves(int x, int y) const {
    int count = 0;
    for (const auto& move : KNIGHT_MOVES) {
        int new_x = x + move.x;
        int new_y = y + move.y;
        if (isValid(new_x, new_y)) {
            count++;
        }
    }
    return count;
}

bool KnightAdvanced::findTourWarnsdorff(int x, int y, int step) {
    board.board[x][y] = step;
    
    if (step == board.size * board.size) {
        return true;
    }
    
    // Собираем все возможные ходы с их "степенями"
    struct MoveOption {
        Position pos;
        int degree;
    };
    
    std::vector<MoveOption> options;
    
    for (const auto& move : KNIGHT_MOVES) {
        int new_x = x + move.x;
        int new_y = y + move.y;
        
        if (isValid(new_x, new_y)) {
            int degree = countAvailableMoves(new_x, new_y);
            options.push_back({{new_x, new_y}, degree});
        }
    }
    // TODO: O(n log n) где n - размер доски, но тк n <= 8: то можем считать О(1)
    // Сортируем по возрастанию степени (правило Варнсдорфа)
    std::sort(options.begin(), options.end(), 
              [](const MoveOption& a, const MoveOption& b) {
                  return a.degree < b.degree;
              });
    

    // TODO: O(n) где n - размер доски, но тк n <= 8: то можем считать О(1)
    // Пробуем ходы в порядке от наименьшей степени
    for (const auto& option : options) {
        if (findTourWarnsdorff(option.pos.x, option.pos.y, step + 1)) {
            return true;
        }
    }
    
    // Если не нашли решение - backtrack
    board.board[x][y] = -1;
    return false;
}

bool KnightAdvanced::solve(int start_x, int start_y) {
    return findTourWarnsdorff(start_x, start_y, 1);
}

void KnightAdvanced::printSolution() const {
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

// TODO:
// T(n) = стоимость_посещения(n² клеток)

// T(n) = n² × [проверка_8_ходов + подсчет_степеней + сортировка_8_элементов]

// T(n) = n² × [O(8) + O(8) + O(8×log₂8)] 
//       = n² × [O(1) + O(1) + O(8×3)]
//       = n² × O(1) 
//       = O(n²)