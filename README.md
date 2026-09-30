# BST Dictionary (AVL) — Spell Checker
**CSE127 · Data Structures (1) · Spring 2026**  
Alexandria University — Faculty of Engineering

---

## Overview

A spell-checking system that loads an English dictionary into an **AVL Tree** and checks user-entered sentences word by word. For each misspelled word, three suggestions are provided based on the BST traversal path.

This implements the **bonus requirement**: using an AVL Tree instead of a plain BST to guarantee O(log n) search and insert operations regardless of insertion order.

---

## How It Works

### Data Structure: AVL Tree

Words are stored in a self-balancing AVL Tree. After every insert, the tree rebalances itself using rotations to maintain the height invariant:

```
|balance_factor(node)| ≤ 1   for every node
balance_factor = height(left) - height(right)
```

With 97,462 words loaded, the resulting tree has:
- **Height = 20** (AVL balanced)
- vs. Height = 38 for an unbalanced BST on the same data

The theoretical AVL upper bound is `1.44 × log₂(n) ≈ 24`, so height 20 is well within the guarantee.

### Suggestions Algorithm

When a word is not found, the search terminates at the deepest node visited before falling off the tree. Call this node **L** (last).

```
Search "wrot":
  root → ... → "wrote"  (go left, NULL)
                  ↑
                  L = "wrote"
```

Three suggestions are printed:

| Label | Definition | How to find |
|-------|-----------|-------------|
| **A** | Node L itself | Tracked during iterative search |
| **B** | Inorder predecessor of L | Largest key < L, found by BST traversal from root |
| **C** | Inorder successor of L | Smallest key > L, found by BST traversal from root |

If no predecessor or successor exists (word is smaller/larger than the entire dictionary), `N/A` is printed.

> **Note:** Suggestions differ slightly from a plain-BST run because AVL rotations produce a different tree shape, changing which node L lands on. The suggestions are always alphabetically adjacent to the misspelled word — the behavior is correct either way.

---

## File Structure

```
.
├── dictionary.c     # Full source code (single file as required)
├── Dictionary.txt   # Word list — one word per line (97,462 words)
└── README.md
```

---

## Build & Run

**Compile:**
```bash
gcc -O2 -o dictionary dictionary.c
```

**Run:**
```bash
./dictionary
```

> `Dictionary.txt` must be in the same directory as the executable.

**Sample session:**
```
Dictionary Loaded Successfully...!
.........................
Size = 97462
.........................
Height = 20
.........................
Enter a sentence (or 0 to quit): I wrot ths assignmet mysel
I - CORRECT
wrot - Incorrect, Suggestions : wrote wrongs wroth
ths - Incorrect, Suggestions : Thucydides thruways Thucydides's
assignmet - Incorrect, Suggestions : assigns assignments assimilate
mysel - Incorrect, Suggestions : mys myrtles myself
Enter a sentence (or 0 to quit): 0
```

---

## Input Handling

| Behavior | Detail |
|----------|--------|
| **Case-insensitive** | `strcasecmp` is used throughout — `HELLO`, `Hello`, `hello` all match |
| **Punctuation stripping** | Leading and trailing `.,;:?!"'` are removed before lookup — `"hello,"` → `hello` |
| **Empty tokens** | Words that become empty after stripping are silently skipped |
| **Multiple spaces** | `strtok` with space/tab delimiters handles any spacing between words |
| **Exit command** | Entering `0` on a line by itself terminates the program |
| **`0` inside a sentence** | Treated as a regular (misspelled) word, not as an exit command |
| **Memory safety** | `malloc` failures exit with an error message; `ferror` is checked after dictionary load |

---

## Complexity

| Operation | Complexity |
|-----------|-----------|
| Insert (AVL) | O(log n) |
| Search | O(log n) |
| Inorder predecessor / successor | O(log n) |
| Load dictionary (n words) | O(n log n) |
| Memory | O(n) |

---

## Functions Reference

```c
// AVL core
Node *insert(Node *root, const char *word);

// Search — returns found node or NULL, sets *last to deepest node visited
Node *search(Node *root, const char *word, Node **last);

// Suggestion helpers — both O(log n), no parent pointers needed
Node *inorder_successor  (Node *root, const char *word);
Node *inorder_predecessor(Node *root, const char *word);

// Utilities
int   tree_size(Node *root);
void  free_tree(Node *root);
```

---

## Requirements

- C compiler: GCC (Linux / macOS / MinGW on Windows) or MSVC
- > `Dictionary.txt` (one word per line) is not included in this repository. Place your own word list in the same directory as the executable.

**Windows note:** the code uses `_stricmp` automatically on Windows via a compile-time macro — no changes needed.

## Author
Omar El-Banna, CCE, Alexandria University, Faculty of Engineering.

## License
Released under the MIT License. Developed as a course project for Data Structures (1), Faculty of Engineering, Alexandria University.
