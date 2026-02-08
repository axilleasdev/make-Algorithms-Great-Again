# 🌳 Make Algorithms Great Again

**Data Structures & Algorithms (AG206) — Practical Assignment**

Implementation and benchmark comparison of three tree data structures in C:
**Binary Search Tree (BST)**, **AVL Tree**, and **Red-Black Tree**.

Each tree stores US citizen records and is benchmarked for insert, search, and delete performance.

---

## 📁 Project Structure

```
├── generate_citizens.c   # Dataset generator (reads names.txt + states.txt → citizens.txt)
├── bst.c                 # Binary Search Tree implementation
├── avl.c                 # AVL Tree implementation
├── redblack.c            # Red-Black Tree implementation
├── benchmark.c           # Performance measurement & comparison
├── names.txt             # 1000 random full names (from 1000randomnames.com)
├── states.txt            # 50 US states
├── citizens.txt          # Generated dataset (1000 records)
├── results.txt           # Benchmark results table
└── results.png           # Benchmark bar plot
```

## 🗃️ Data Structure

Each citizen record contains:

| Field         | Type       | Example          |
|---------------|------------|------------------|
| Full Name     | `char[100]`| Royal Galindo    |
| Age           | `int`      | 30               |
| State         | `char[50]` | New York         |
| Annual Income | `int`      | 100000           |

The **full name** serves as the unique key for all tree operations.

## 🔧 Compilation & Execution

### 1. Generate the dataset

```bash
gcc -o generate_citizens generate_citizens.c
./generate_citizens
```

This reads `names.txt` and `states.txt` and produces `citizens.txt` with 1000 random citizen records.

### 2. Run the benchmark

```bash
gcc -O2 -o benchmark benchmark.c
./benchmark
```

This inserts 1000 records, searches 100 random names, and deletes 100 random names across all three trees. Each test is repeated 5 times and averaged. Results are saved to `results.txt`.

## 📊 Benchmark Results

| Operation | BST (μs) | AVL (μs) | RBT (μs) |
|-----------|----------|----------|----------|
| Insert    | 153.60   | 154.80   | 124.00   |
| Search    | 0.20     | 0.00     | 0.00     |
| Delete    | 14.40    | 14.00    | 17.00    |

*Average of 5 runs — 1000 inserts, 100 searches, 100 deletions*

## 🧠 Tree Implementations

### BST (`bst.c`)
Standard binary search tree. Simple insertion/deletion by comparing names. No balancing — worst case O(n) if data is sorted.

### AVL Tree (`avl.c`)
Self-balancing BST that maintains a height balance factor of ±1. Uses four rotation types (Left, Right, Left-Right, Right-Left) after every insert and delete.

### Red-Black Tree (`redblack.c`)
Self-balancing BST using node coloring (Red/Black) with five properties that guarantee O(log n) operations. Uses recoloring and rotations to fix violations after insert and delete.

## 🛠️ Operations Supported

| Operation  | Description                                      |
|------------|--------------------------------------------------|
| `insert`   | Insert a citizen record by full name              |
| `delete`   | Delete a citizen record by full name              |
| `search`   | Find a citizen by full name                       |
| `traversal`| In-order traversal (alphabetical output to file)  |

## 📚 References

- Cormen, T.H. et al., *Introduction to Algorithms*, MIT Press
- [Programiz — BST](https://www.programiz.com/dsa/binary-search-tree)
- [Programiz — AVL Tree](https://www.programiz.com/dsa/avl-tree)
- [Programiz — Red-Black Tree](https://www.programiz.com/dsa/red-black-tree)
- [GeeksforGeeks — Red-Black Tree](https://www.geeksforgeeks.org/introduction-to-red-black-tree/)
- [USF — AVL Tree Visualization](https://www.cs.usfca.edu/~galles/visualization/AVLtree.html)

---

**University of Essex — BSc Computing (Artificial Intelligence)**
Module: Data Structures and Algorithms (AG206) | 2025–2026
