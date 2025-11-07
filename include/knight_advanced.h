#pragma once

#include "base.h"

struct KnightAdvanced {
    KnightBoard board;
    
    KnightAdvanced(int size) : board(size) {}
    
    bool solve(int start_x = 0, int start_y = 0);
    void printSolution() const;

private:
    bool findTourWarnsdorff(int x, int y, int step);
    bool isValid(int x, int y) const;
    int countAvailableMoves(int x, int y) const;
};