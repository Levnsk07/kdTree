#!/usr/bin/env python3
import sys
import csv
import matplotlib.pyplot as plt


def read_points(filename):
    xs = []
    ys = []

    with open(filename, newline="", encoding="utf-8") as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) < 2:
                continue
            try:
                xs.append(float(row[0]))
                ys.append(float(row[1]))
            except ValueError:
                pass

    return xs, ys


def main():
    if len(sys.argv) != 2:
        print("Использование: python whatch.py lidar.csv")
        sys.exit(1)

    xs, ys = read_points(sys.argv[1])

    plt.figure(figsize=(8, 8))
    plt.scatter(xs, ys, s=10)
    plt.grid(True)
    plt.axis("equal")

    # Вместо plt.show()
    plt.savefig("points.png", dpi=300, bbox_inches="tight")
    print("График сохранён в points.png")


if __name__ == "__main__":
    main()