#include "knight_advanced.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <climits>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

struct Measurement {
    int board_size;
    long long time_microseconds;
    int total_cells;
    bool success;
};

class ComplexityBenchmark {
private:
    std::vector<Measurement> measurements;
    std::string data_dir = "data";
    
    bool createDirectory(const std::string& path) {
#ifdef _WIN32
        return CreateDirectoryA(path.c_str(), NULL) != 0;
#else
        return mkdir(path.c_str(), 0755) == 0;
#endif
    }
    
    bool directoryExists(const std::string& path) {
#ifdef _WIN32
        DWORD attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#else
        struct stat info;
        return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
#endif
    }
    
public:
    ComplexityBenchmark() {
        // Создаем папку data если ее нет
        if (!directoryExists(data_dir)) {
            if (createDirectory(data_dir)) {
                std::cout << "Created directory: " << data_dir << std::endl;
            } else {
                std::cout << "Warning: Could not create directory " << data_dir << std::endl;
                data_dir = "."; // используем текущую директорию
            }
        }
    }
    
    void runBenchmark(int min_size, int max_size, int step = 1, int samples_per_size = 3) {
        std::cout << "Running complexity benchmark from size " << min_size 
                  << " to " << max_size << std::endl;
        std::cout << "Samples per size: " << samples_per_size << std::endl;
        
        for (int size = min_size; size <= max_size; size += step) {
            std::cout << "\nTesting board size: " << size << "x" << size << std::endl;
            
            // Делаем несколько замеров для усреднения
            long long total_time = 0;
            int successful_runs = 0;
            long long min_time = LLONG_MAX;
            long long max_time = 0;
            
            for (int sample = 0; sample < samples_per_size; sample++) {
                Measurement result = measureAlgorithm(size);
                if (result.success) {
                    total_time += result.time_microseconds;
                    successful_runs++;
                    min_time = std::min(min_time, result.time_microseconds);
                    max_time = std::max(max_time, result.time_microseconds);
                    std::cout << "  Sample " << (sample + 1) << ": " 
                              << result.time_microseconds << " μs" << std::endl;
                } else {
                    std::cout << "  Sample " << (sample + 1) << ": FAILED" << std::endl;
                }
            }
            
            if (successful_runs > 0) {
                long long avg_time = total_time / successful_runs;
                measurements.push_back({size, avg_time, size * size, true});
                std::cout << "  Average: " << avg_time << " μs, Range: [" 
                          << min_time << "-" << max_time << "] μs ("
                          << successful_runs << "/" << samples_per_size << " successful)" << std::endl;
            } else {
                measurements.push_back({size, 0, size * size, false});
                std::cout << "  All samples failed for this size" << std::endl;
            }
        }
        
        if (saveToCSV()) {
            std::cout << "\n✓ Data successfully saved to " << data_dir << "/knight_complexity.csv" << std::endl;
        } else {
            std::cout << "\n✗ Failed to save data file!" << std::endl;
        }
        
        printAnalysis();
    }
    
private:
    Measurement measureAlgorithm(int board_size) {
        KnightAdvanced solver(board_size);
        
        // Используем стандартную стартовую позицию (0,0)
        int start_x = 0, start_y = 0;
        
        auto start = std::chrono::high_resolution_clock::now();
        bool success = solver.solve(start_x, start_y);
        auto end = std::chrono::high_resolution_clock::now();
        
        long long time_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        return {board_size, time_us, board_size * board_size, success};
    }
    
    bool saveToCSV() {
        std::string filename = data_dir + "/knight_complexity.csv";
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "Error: Could not create file " << filename << std::endl;
            return false;
        }
        
        // Заголовок CSV
        file << "board_size,total_cells,time_microseconds,success,time_per_cell" << std::endl;
        
        // Данные
        for (const auto& m : measurements) {
            double time_per_cell = m.success ? static_cast<double>(m.time_microseconds) / m.total_cells : 0.0;
            file << m.board_size << ","
                 << m.total_cells << ","
                 << m.time_microseconds << ","
                 << m.success << ","
                 << time_per_cell << std::endl;
        }
        
        file.close();
        return true;
    }
    
    void printAnalysis() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "COMPLEXITY ANALYSIS RESULTS" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        if (measurements.size() < 2) {
            std::cout << "Not enough data for analysis" << std::endl;
            return;
        }
        
        // Таблица с данными
        std::cout << "\nDetailed Measurements:" << std::endl;
        std::cout << "Size\tCells\tTime(μs)\tμs/Cell\tComplexity" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        for (size_t i = 0; i < measurements.size(); i++) {
            const auto& m = measurements[i];
            if (!m.success) continue;
            
            double time_per_cell = static_cast<double>(m.time_microseconds) / m.total_cells;
            std::cout << m.board_size << "\t" 
                      << m.total_cells << "\t"
                      << m.time_microseconds << "\t\t"
                      << time_per_cell << "\t";
            
            // Простая оценка сложности по времени на клетку
            if (time_per_cell < 1.0) std::cout << "FAST";
            else if (time_per_cell < 10.0) std::cout << "MEDIUM";
            else std::cout << "SLOW";
            
            std::cout << std::endl;
        }
        
        // Анализ роста сложности
        std::cout << "\nComplexity Growth Analysis:" << std::endl;
        std::cout << "From→To\tCells×\tTime×\tRatio\tVerdict" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        double sum_ratios = 0;
        int ratio_count = 0;
        
        for (size_t i = 1; i < measurements.size(); i++) {
            const auto& prev = measurements[i-1];
            const auto& curr = measurements[i];
            
            if (!prev.success || !curr.success) continue;
            
            double cells_growth = static_cast<double>(curr.total_cells) / prev.total_cells;
            double time_growth = static_cast<double>(curr.time_microseconds) / prev.time_microseconds;
            double ratio = time_growth / cells_growth;
            
            sum_ratios += ratio;
            ratio_count++;
            
            std::cout << prev.board_size << "→" << curr.board_size << "\t"
                      << cells_growth << "\t"
                      << time_growth << "\t"
                      << ratio << "\t";
            
            if (ratio >= 0.7 && ratio <= 1.3) {
                std::cout << "O(n²) ✓";
            } else if (ratio >= 1.3 && ratio <= 2.0) {
                std::cout << "O(n³) ⚠";
            } else if (ratio > 2.0) {
                std::cout << ">O(n³) ✗";
            } else {
                std::cout << "<?>";
            }
            
            std::cout << std::endl;
        }
        
        // Финальный вывод
        if (ratio_count > 0) {
            double avg_ratio = sum_ratios / ratio_count;
            
            std::cout << "\n" << std::string(50, '=') << std::endl;
            std::cout << "FINAL COMPLEXITY ASSESSMENT" << std::endl;
            std::cout << std::string(50, '=') << std::endl;
            std::cout << "Average Time/Cells Growth Ratio: " << avg_ratio << std::endl;
            
            if (avg_ratio >= 0.8 && avg_ratio <= 1.2) {
                std::cout << "✓ CONFIRMED: O(n²) complexity" << std::endl;
                std::cout << "  - Time grows linearly with number of cells" << std::endl;
                std::cout << "  - Algorithm scales efficiently" << std::endl;
            } else if (avg_ratio >= 0.5 && avg_ratio <= 1.5) {
                std::cout << "✓ LIKELY: O(n²) complexity" << std::endl;
                std::cout << "  - Time growth close to linear with cells" << std::endl;
            } else if (avg_ratio >= 1.5 && avg_ratio <= 2.5) {
                std::cout << "⚠ POSSIBLE: O(n³) complexity" << std::endl;
                std::cout << "  - Time grows faster than number of cells" << std::endl;
            } else if (avg_ratio > 2.5) {
                std::cout << "✗ DETECTED: Worse than O(n³)" << std::endl;
                std::cout << "  - Time grows much faster than expected" << std::endl;
            } else {
                std::cout << "? UNCLEAR: Inconsistent growth pattern" << std::endl;
            }
        }
        
        // Показываем где файл
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "DATA EXPORT" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "Results saved to: " << data_dir << "/knight_complexity.csv" << std::endl;
        std::cout << "Format: board_size, total_cells, time_microseconds, success, time_per_cell" << std::endl;
    }
};

int main() {
    ComplexityBenchmark benchmark;
    
    std::cout << "KNIGHT'S TOUR COMPLEXITY BENCHMARK" << std::endl;
    std::cout << "==================================" << std::endl;
    
    int choice;
    std::cout << "\nChoose benchmark mode:" << std::endl;
    std::cout << "1. Quick test (sizes 5-12, fast)" << std::endl;
    std::cout << "2. Standard test (sizes 5-20, recommended)" << std::endl;
    std::cout << "3. Stress test (sizes 5-30, slow)" << std::endl;
    std::cout << "4. Custom range" << std::endl;
    std::cout << "Enter choice (1-4): ";
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            benchmark.runBenchmark(5, 12, 1, 2);
            break;
        case 2:
            benchmark.runBenchmark(5, 20, 1, 3);
            break;
        case 3:
            benchmark.runBenchmark(5, 30, 1, 2);
            break;
        case 4:
            int min_size, max_size, samples;
            std::cout << "Enter min board size: ";
            std::cin >> min_size;
            std::cout << "Enter max board size: ";
            std::cin >> max_size;
            std::cout << "Enter samples per size: ";
            std::cin >> samples;
            benchmark.runBenchmark(min_size, max_size, 1, samples);
            break;
        default:
            std::cout << "Invalid choice, running standard test" << std::endl;
            benchmark.runBenchmark(5, 20, 1, 3);
    }
    
    return 0;
}