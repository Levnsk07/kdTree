import pandas as pd
import matplotlib.pyplot as plt

def plot_points_from_csv(csv_file, x_col='x', y_col='y', label_col=None, title='Scatter Plot of Points', output_file='points_plot.png'):
    """
    Построение графика точек из CSV-файла.

    :param csv_file: путь к CSV-файлу
    :param x_col: имя столбца с координатами X
    :param y_col: имя столбца с координатами Y
    :param label_col: имя столбца с метками (если есть)
    :param title: заголовок графика
    :param output_file: файл для сохранения графика
    """
    # Чтение данных из CSV
    data = pd.read_csv(csv_file)

    # Проверка наличия нужных столбцов
    if x_col not in data.columns or y_col not in data.columns:
        raise ValueError(f"CSV-файл должен содержать столбцы '{x_col}' и '{y_col}'")

    # Определение цветовой схемы
    colors = None
    if label_col and label_col in data.columns:
        colors = data[label_col]

    # Построение графика
    plt.figure(figsize=(10, 6))
    scatter = plt.scatter(data[x_col], data[y_col], c=colors, cmap='viridis', alpha=0.7)

    # Легенда (если есть метки)
    if label_col and label_col in data.columns:
        plt.colorbar(scatter, label=label_col)

    plt.title(title)
    plt.xlabel(x_col)
    plt.ylabel(y_col)
    plt.grid(True)

    # Сохранение графика
    plt.savefig(output_file)
    print(f"График сохранён в файл: {output_file}")
    plt.show()

# Пример использования
if __name__ == "__main__":
    # Укажите путь к вашему CSV-файлу
    csv_file = "points.csv"  # Замените на ваш файл

    # Пример CSV-файла (points.csv):
    # x,y,cluster
    # 1.0,1.0,0
    # 1.5,1.8,0
    # 5.0,8.0,1
    # 8.0,8.0,1
    # 3.0,3.0,-1

    plot_points_from_csv(
        csv_file=csv_file,
        x_col='x',
        y_col='y',
        label_col='cluster',  # Укажите столбец с метками (если есть)
        title='DBSCAN Clustering Result',
        output_file='dbscan_plot.png'
    )