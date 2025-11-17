import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

def plot_complexity():
    # Читаем данные
    df = pd.read_csv('data/knight_complexity.csv')
    
    # Фильтруем только успешные запуски
    df_success = df[df['success'] == True]
    
    if len(df_success) < 2:
        print("Not enough successful measurements for analysis")
        return
    
    # Создаем графики
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
    
    # График 1: Время vs Размер доски
    ax1.plot(df_success['board_size'], df_success['time_microseconds'], 'bo-', linewidth=2, markersize=6)
    ax1.set_xlabel('Board Size (n)')
    ax1.set_ylabel('Time (microseconds)')
    ax1.set_title('Execution Time vs Board Size')
    ax1.grid(True, alpha=0.3)
    
    # График 2: Время vs Количество клеток (n²)
    ax2.plot(df_success['total_cells'], df_success['time_microseconds'], 'ro-', linewidth=2, markersize=6)
    ax2.set_xlabel('Total Cells (n²)')
    ax2.set_ylabel('Time (microseconds)')
    ax2.set_title('Execution Time vs Total Cells\n(Should be linear for O(n²))')
    ax2.grid(True, alpha=0.3)
    
    # График 3: Логарифмический масштаб
    ax3.loglog(df_success['total_cells'], df_success['time_microseconds'], 'go-', linewidth=2, markersize=6, label='Actual data')
    ax3.set_xlabel('Total Cells (n²)')
    ax3.set_ylabel('Time (microseconds)')
    ax3.set_title('Log-Log Plot')
    ax3.grid(True, alpha=0.3)
    
    # Линии тренда для сравнения
    # O(n²) reference
    x_fit = np.linspace(df_success['total_cells'].min(), df_success['total_cells'].max(), 100)
    y_n2 = df_success['time_microseconds'].iloc[0] * (x_fit / df_success['total_cells'].iloc[0])
    ax3.loglog(x_fit, y_n2, 'r--', alpha=0.7, label='O(n²) reference')
    
    # O(n³) reference  
    y_n3 = df_success['time_microseconds'].iloc[0] * (x_fit / df_success['total_cells'].iloc[0]) ** 1.5
    ax3.loglog(x_fit, y_n3, 'b--', alpha=0.7, label='O(n³) reference')
    
    ax3.legend()
    
    # График 4: Время на клетку (должно быть константой для O(n²))
    ax4.plot(df_success['board_size'], df_success['time_per_cell'], 'mo-', linewidth=2, markersize=6)
    ax4.set_xlabel('Board Size (n)')
    ax4.set_ylabel('Time per Cell (μs/cell)')
    ax4.set_title('Time per Cell\n(Should be constant for O(n²))')
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('data/complexity_analysis.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # Статистика и анализ сложности
    print("=== COMPLEXITY ANALYSIS ===")
    print(f"Data points: {len(df_success)}")
    print(f"Board sizes: {df_success['board_size'].min()} to {df_success['board_size'].max()}")
    print(f"Time range: {df_success['time_microseconds'].min():.2f} to {df_success['time_microseconds'].max():.2f} μs")
    
    # Анализ роста сложности
    print("\nGrowth Analysis:")
    print("Size(n)\tCells\tTime(μs)\tμs/Cell\tGrowth Ratio")
    print("-" * 60)
    
    ratios = []
    for i in range(1, len(df_success)):
        prev = df_success.iloc[i-1]
        curr = df_success.iloc[i]
        
        cells_growth = curr['total_cells'] / prev['total_cells']
        time_growth = curr['time_microseconds'] / prev['time_microseconds']
        ratio = time_growth / cells_growth
        ratios.append(ratio)
        
        print(f"{prev['board_size']}→{curr['board_size']}\t"
              f"{cells_growth:.1f}x\t{time_growth:.1f}x\t"
              f"{curr['time_per_cell']:.3f}\t{ratio:.3f}")
    
    if ratios:
        avg_ratio = np.mean(ratios)
        std_ratio = np.std(ratios)
        
        print(f"\nAverage Growth Ratio: {avg_ratio:.3f} ± {std_ratio:.3f}")
        
        if 0.8 <= avg_ratio <= 1.2:
            print("CONCLUSION: O(n²) complexity ✓")
            print("- Time grows linearly with board area")
        elif 1.3 <= avg_ratio <= 2.0:
            print("CONCLUSION: O(n³) complexity ⚠")
            print("- Time grows faster than board area")
        elif avg_ratio > 2.0:
            print("CONCLUSION: Worse than O(n³) ✗")
            print("- Time grows much faster than expected")
        else:
            print("CONCLUSION: Better than O(n²) ?")
            print("- Time grows slower than board area")

if __name__ == "__main__":
    if os.path.exists('data/knight_complexity.csv'):
        plot_complexity()
    else:
        print("Error: Data file not found. Run the C++ benchmark first.")
        print("Expected file: data/knight_complexity.csv")