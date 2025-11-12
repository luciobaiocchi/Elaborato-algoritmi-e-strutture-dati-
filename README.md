# Shortest Path Calculation (BFS) - Algorithms and Data Structures Project

This project is part of the **Algorithms and Data Structures** course at the University of Bologna, Academic Year 2022/2023. The goal is to implement an efficient algorithm to find the shortest path in a grid-based city map, avoiding buildings and minimizing exposure to rain.

---

## Problem Description

The city map is represented as a grid of size `N x M`, where each cell contains a character `'0'` to `'9'`:
- `'0'` represents a sidewalk (walkable area).
- `'1'` to `'9'` represent buildings with heights from 1 to 9 floors.

Rain falls from left to right at a 45-degree angle. A building of height `c` blocks rain for the next `c` cells to its right. A pedestrian starts at the top-left cell `(0, 0)` and must reach the bottom-right cell `(N-1, M-1)` by moving to adjacent cells (up, down, left, or right), avoiding buildings. The goal is to find the shortest path that minimizes the number of cells exposed to rain.

---

## Input Format

The input file contains:
1. The first line: two integers `N` (rows) and `M` (columns), where `10 ≤ N, M ≤ 500`.
2. The next `N` lines: each line contains `M` characters (`'0'` to `'9'`), representing the grid.

---

## Output Format

The program outputs:
1. If the destination is reachable:
   - The first line: two integers `d` (minimum number of cells visited) and `r` (number of cells exposed to rain).
   - The second line: a sequence of `(d - 1)` characters representing the path (`E` for East, `O` for West, `S` for South, `N` for North).
2. If the destination is unreachable:
   - A single line: `-1 -1`.

---

## Compilation and Execution

### Compilation
To compile the program, use the following command:
```bash
gcc -std=c90 -Wall -Wpedantic 0001079183.c -o 0001079183
```
and to run a test use 
```bash
./0001079183 [test_name]
```