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

With 163,933 unique words loaded, the resulting tree has **Height = 20** (alphabetical `Dictionary.txt`) or **Height = 21** (shuffled `Dictionary_random.txt`).

The theoretical AVL upper bound is `1.44 × log₂(n) ≈ 25`, so both heights are well within the guarantee. See the next section for the comparison with a plain BST.

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

## AVL vs Plain BST: Why Insertion Order Matters

`bst_dictionary.c` is the same program without any balancing, added as a baseline. Both programs were run on the same two word lists (same 163,933 unique words, different order):

| Word list | Order | Plain BST height | AVL height |
|-----------|-------|------------------|------------|
| `Dictionary.txt` | Almost alphabetical (92 out-of-order spots) | **2,874** | **20** |
| `Dictionary_random.txt` | Shuffled (fixed seed) | **42** | **21** |

**What the numbers show**

- A plain BST has no way to fix its shape, so its height is decided entirely by the insertion order. Nearly sorted input is the worst case: each new word is larger than the previous ones and gets attached at the bottom-right, so the tree stretches into a long chain. A search can then need thousands of comparisons (up to 2,874 here).
- With shuffled input the plain BST stays logarithmic, but it is still about twice as tall as the AVL Tree (42 vs 21).
- The AVL Tree rotates after every insert, so its height stays at 20 to 21 whatever the order.

**Why the course word list was probably shuffled** (our interpretation, not confirmed by the course staff)

With an alphabetical list, the plain BST in the assignment would degenerate into a near-chain and every lookup would be painfully slow, so the baseline would say more about the worst case than about a normal BST. A shuffled list gives the plain BST a reasonable, typical height (around 40), so the comparison with the AVL bonus is fair: the AVL Tree is still clearly better (about half the height), without the result being exaggerated by a worst-case input.

**Reproduce the table**

```bash
gcc -O2 -o dictionary     dictionary.c
gcc -O2 -o bst_dictionary bst_dictionary.c

echo 0 | ./bst_dictionary Dictionary.txt        # Height = 2874
echo 0 | ./bst_dictionary Dictionary_random.txt # Height = 42
echo 0 | ./dictionary     Dictionary.txt        # Height = 20
echo 0 | ./dictionary     Dictionary_random.txt # Height = 21
```

---

## File Structure

```
.
├── dictionary.c             # AVL spell checker (main program)
├── bst_dictionary.c         # Plain BST version (no balancing), used as a comparison baseline
├── Dictionary.txt           # Word list, one word per line (166,817 lines, 163,933 unique ignoring case)
├── Dictionary_random.txt    # Same words in shuffled order (fixed seed)
├── Dictionary-LICENSE.txt   # Copyright and permission notice for the word list
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

> `Dictionary.txt` must be in the same directory as the executable (the file name is case-sensitive on Linux). Both programs also accept an optional word list path, for example `./dictionary Dictionary_random.txt`.

**Sample session:**
```
Dictionary Loaded Successfully...!
.........................
Size = 163933
.........................
Height = 20
.........................
Enter a sentence (or 0 to quit): I wrot ths assignmet mysel
I - CORRECT
wrot - Incorrect, Suggestions : wrote wrongs wroth
ths - Incorrect, Suggestions : Thu thruways Thuban
assignmet - Incorrect, Suggestions : assignments assignment's assignor
mysel - Incorrect, Suggestions : myself myrtles Mysore
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

The O(log n) figures hold for the AVL Tree in every case. A plain BST only gives O(log n) on average for shuffled input and degrades toward O(n) on sorted input.

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
- `Dictionary.txt` in the working directory (included)

**Windows note:** the code uses `_stricmp` automatically on Windows via a compile-time macro — no changes needed.

---

## Word List

`Dictionary.txt` was generated with the [ESDB / SCOWL](https://wordlist.aspell.net) word list generator (US spelling, size 70, diacritics stripped, hacker words included). It contains proper nouns and possessives (for example `Mysore` and `assignment's`), so suggestions may include them. The full copyright and permission notice is in `Dictionary-LICENSE.txt`.

---

## Author

Omar El-Banna, Computer and Communication Engineering, Faculty of Engineering, Alexandria University.

## License

The source code is released under the MIT License. Developed as a course project for Data Structures (1), Faculty of Engineering, Alexandria University. The word list is governed by the notice in `Dictionary-LICENSE.txt`.
