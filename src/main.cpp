#include "knight_basic.h"
#include "knight_advanced.h"
#include <iostream>
#include <chrono>
#include <cmath>

void printComplexityAnalysis(int board_size) {
    std::cout << "\n=== ALGORITHM COMPLEXITY ANALYSIS ===" << std::endl;
    
    int n = board_size;
    int total_cells = n * n;
    
    std::cout << "Board size: " << n << "x" << n << " (" << total_cells << " cells)" << std::endl;
    
    // Анализ простого backtracking
    std::cout << "\n1. BASIC BACKTRACKING ALGORITHM:" << std::endl;
    std::cout << "   - Worst-case time: O(8^(N²)) = O(8^" << total_cells << ")" << std::endl;
    std::cout << "   - Space complexity: O(N²) for recursion stack" << std::endl;
    std::cout << "   - Branching factor: 8 (knight moves)" << std::endl;
    std::cout << "   - States to explore: ~8^" << total_cells << " ≈ ";

    // Вычисляем приблизительное число состояний (только для маленьких n)
    if (total_cells <= 16) {
        double states = std::pow(8, total_cells);
        if (states < 1e12) {
            std::cout << states;
        } else {
            std::cout << "10^" << static_cast<int>(std::log10(states));
        }
    } else {
        std::cout << "10^" << static_cast<int>(total_cells * std::log10(8));
    }
    std::cout << " states" << std::endl;
    
    // Анализ улучшенного алгоритма
    std::cout << "\n2. WARNSDORFF'S HEURISTIC ALGORITHM:" << std::endl;
    std::cout << "   - Average time: O(N²) to O(N³)" << std::endl;
    std::cout << "   - Space complexity: O(N²)" << std::endl;
    std::cout << "   - Key optimization: greedy choice by minimum degree" << std::endl;
    std::cout << "   - Reduces branching factor from 8 to ~2-3" << std::endl;
    std::cout << "   - States to explore: ~" << total_cells << " steps" << std::endl;
    
    // Сравнение
    std::cout << "\n3. PERFORMANCE GAIN:" << std::endl;
    if (total_cells <= 16) {
        double basic_states = std::pow(8, total_cells);
        double advanced_states = total_cells;
        double improvement = basic_states / advanced_states;
        std::cout << "   - State reduction: " << improvement << "x" << std::endl;
    }
    std::cout << "   - Practical difference:" << std::endl;
    std::cout << "     * N=5: Basic ~100ms, Advanced ~0ms" << std::endl;
    std::cout << "     * N=6: Basic ~seconds-minutes, Advanced ~0ms" << std::endl;
    std::cout << "     * N=8: Basic ~years, Advanced ~0ms" << std::endl;
}

int main() {
    int board_size;
    std::cout << "Enter board size: ";
    std::cin >> board_size;
    
    int start_x, start_y;
    std::cout << "Enter start position (x y): ";
    std::cin >> start_x >> start_y;
    
    // Сначала выводим анализ сложности
    printComplexityAnalysis(board_size);
    
    // Переменные для времени
    long long advanced_time = 0;
    long long basic_time = -1;
    bool advanced_success = false;
    bool basic_success = false;
    
    std::cout << "\n=== EXECUTION RESULTS ===" << std::endl;
    
    // Тестируем улучшенный метод
    {
        KnightAdvanced solver_advanced(board_size);
        
        auto start = std::chrono::high_resolution_clock::now();
        advanced_success = solver_advanced.solve(start_x, start_y);
        auto end = std::chrono::high_resolution_clock::now();
        advanced_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        if (advanced_success) {
            std::cout << "✓ Advanced method: " << advanced_time << " μs" << std::endl;
            solver_advanced.printSolution();
        } else {
            std::cout << "✗ Advanced method: No solution found" << std::endl;
        }
    }
    
    // Тестируем простой метод с ограничением по времени
    if (board_size <= 6) {
        KnightBasic solver_basic(board_size);
        
        auto start = std::chrono::high_resolution_clock::now();
        basic_success = solver_basic.solve(start_x, start_y);
        auto end = std::chrono::high_resolution_clock::now();
        basic_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        if (basic_success) {
            std::cout << "✓ Basic method: " << basic_time << " ms" << std::endl;
        } else {
            std::cout << "✗ Basic method: No solution found" << std::endl;
        }
    } else {
        std::cout << "⏭️  Basic method: Skipped (N > 6 would take too long)" << std::endl;
    }
    
    // Сравниваем производительность
    std::cout << "\n=== PERFORMANCE SUMMARY ===" << std::endl;
    std::cout << "Advanced method: " << advanced_time << " μs" << std::endl;
    
    if (basic_time != -1) {
        std::cout << "Basic method: " << basic_time << " ms" << std::endl;
        if (advanced_time > 0 && basic_time > 0) {
            double speedup = static_cast<double>(basic_time * 1000) / advanced_time;
            std::cout << "Speedup: " << speedup << "x faster" << std::endl;
        } else if (advanced_time == 0 && basic_time > 0) {
            std::cout << "Speedup: ∞ (advanced method was instant)" << std::endl;
        }
    } else {
        std::cout << "Basic method: Not executed" << std::endl;
        std::cout << "Theoretical speedup: > 1,000,000x for N=" << board_size << std::endl;
    }
    
    return 0;
}