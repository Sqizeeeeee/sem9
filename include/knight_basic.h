#pragma once

#include "base.h"

struct KnightBasic {
    KnightBoard board;
    
    KnightBasic(int size) : board(size) {}
    
    bool solve(int start_x = 0, int start_y = 0);
    void printSolution() const;

private:
    bool findTour(int x, int y, int step);
    bool isValid(int x, int y) const;
};