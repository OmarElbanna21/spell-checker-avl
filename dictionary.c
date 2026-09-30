/*
 * BST Dictionary (AVL) - Spell Checker
 * =====================================
 * CSE127 - Data Structures (1) | Spring 2026
 * 
 * Name: [Omar Ehab Abdelmoneim Mahmoud El-Banna]
 * ID: [11172]
 *
 * OVERVIEW
 * --------
 * Loads an English dictionary (Dictionary.txt) into an AVL Tree,
 * then checks user-entered sentences word by word.
 *
 * For each word:
 *   - Found     -> prints CORRECT
 *   - Not found -> prints 3 suggestions:
 *       A. Last node reached during search
 *       B. Inorder predecessor of A
 *       C. Inorder successor of A
 *
 * BONUS: Uses AVL Tree (self-balancing) instead of plain BST.
 *   - Guarantees O(log n) search and insert
 *   - With 97,462 words: Height = 20  (vs. 38 for unbalanced BST)
 *
 * DESIGN NOTES
 * ------------
 * - Search is iterative (no recursion) => more efficient and avoids stack overflow
 * - strcasecmp / _stricmp used for case-insensitive comparison
 * - Leading and trailing punctuation stripped before lookup
 * - Enter 0 on a blank line to exit
 * - 0 inside a sentence treated as a regular (misspelled) word, not as an exit command
 * 
 * =================
 *  SUBMISSION NOTE
 * =================
 * Submitting source code only (.c) as per requirements.
 * The DESIGN NOTES section above serves as inline documentation
 * in case there is no discussion to elaborate further.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define strcasecmp _stricmp
#else
    #include <strings.h>
#endif

#define MAX_WORD 100

typedef struct Node {
    char word[MAX_WORD];
    int height;
    struct Node *left, *right;
} Node;

/* ── AVL helpers ── */

static int node_height(Node *n) {
    return n ? n->height : 0;
}

static int max2(int a, int b) {
    return a > b ? a : b;
}

static void update_height(Node *n) {
    if (n)
        n->height = 1 + max2(node_height(n->left), node_height(n->right));
}

static int balance_factor(Node *n) {
    return n ? node_height(n->left) - node_height(n->right) : 0;
}

static Node *new_node(const char *word) {
    Node *n = malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }
    strncpy(n->word, word, MAX_WORD - 1);
    n->word[MAX_WORD - 1] = '\0';
    n->height = 1;
    n->left = n->right = NULL;
    return n;
}

static Node *rotate_right(Node *y) {
    Node *x  = y->left;
    Node *T2 = x->right;
    x->right = y;
    y->left  = T2;
    update_height(y);
    update_height(x);
    return x;
}

static Node *rotate_left(Node *x) {
    Node *y  = x->right;
    Node *T2 = y->left;
    y->left  = x;
    x->right = T2;
    update_height(x);
    update_height(y);
    return y;
}

static Node *rebalance(Node *n) {
    update_height(n);
    int bf = balance_factor(n);

    if (bf > 1) {
        if (balance_factor(n->left) < 0)
            n->left = rotate_left(n->left);
        return rotate_right(n);
    }
    if (bf < -1) {
        if (balance_factor(n->right) > 0)
            n->right = rotate_right(n->right);
        return rotate_left(n);
    }
    return n;
}

/* ── Core operations ── */

Node *insert(Node *root, const char *word) {
    if (!root) return new_node(word);
    int cmp = strcasecmp(word, root->word);
    if      (cmp < 0) root->left  = insert(root->left,  word);
    else if (cmp > 0) root->right = insert(root->right, word);
    else              return root;   /* duplicate – ignore */
    return rebalance(root);
}

/* Returns found node (or NULL), sets *last to last visited node */
Node *search(Node *root, const char *word, Node **last) {
    Node *curr = root;
    *last = NULL;
    while (curr) {
        *last = curr;
        int cmp = strcasecmp(word, curr->word);
        if (cmp == 0) return curr;
        curr = (cmp < 0) ? curr->left : curr->right;
    }
    return NULL;
}

/* Inorder successor: smallest key > word  (traversal from root) */
Node *inorder_successor(Node *root, const char *word) {
    Node *succ = NULL, *curr = root;
    while (curr) {
        int cmp = strcasecmp(word, curr->word);
        if (cmp < 0) { succ = curr; curr = curr->left; }
        else           curr = curr->right;
    }
    return succ;
}

/* Inorder predecessor: largest key < word  (traversal from root) */
Node *inorder_predecessor(Node *root, const char *word) {
    Node *pred = NULL, *curr = root;
    while (curr) {
        int cmp = strcasecmp(word, curr->word);
        if (cmp > 0) { pred = curr; curr = curr->right; }
        else           curr = curr->left;
    }
    return pred;
}

int tree_size(Node *root) {
    if (!root) return 0;
    return 1 + tree_size(root->left) + tree_size(root->right);
}

void free_tree(Node *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

/* ── Dictionary loader ── */

Node *load_dictionary(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open '%s'\n", filename);
        exit(1);
    }
    Node *root = NULL;
    char  word[MAX_WORD];
    while (fscanf(f, "%99s", word) == 1)
        root = insert(root, word);
    if (ferror(f)) {
        perror("Error reading dictionary");
        exit(1);
    }
    fclose(f);
    return root;
}

/* Strips leading and trailing punctuation from a word in-place */
static char *strip_punctuation(char *s) {
    int len = strlen(s);
    while (len > 0 && strchr(".,;:?!\"'", s[len - 1]))
        s[--len] = '\0';
    char *start = s;
    while (*start && strchr(".,;:?!\"'", *start))
        start++;
    if (start != s)
        memmove(s, start, strlen(start) + 1);
    return s;
}


int main() {
    Node *root = load_dictionary("Dictionary.txt");

    printf("Dictionary Loaded Successfully...!\n");
    printf(".........................\n");
    printf("Size = %d\n", tree_size(root));
    printf(".........................\n");
    printf("Height = %d\n", node_height(root));
    printf(".........................\n");

    char line[1024];
    while (1) {
        printf("Enter a sentence (or 0 to quit): ");
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n\r")] = '\0';
        if (strcmp(line, "0") == 0) break;

        char *token = strtok(line, " \t");
        while (token) {
            strip_punctuation(token);
            if (*token == '\0') { token = strtok(NULL, " \t"); continue; }

            Node *last  = NULL;
            Node *found = search(root, token, &last);

            if (found) {
                printf("%s - CORRECT\n", token);
            } else {
                Node *pred = last ? inorder_predecessor(root, last->word) : NULL;
                Node *succ = last ? inorder_successor  (root, last->word) : NULL;

                printf("%s - Incorrect, Suggestions : %s %s %s\n",
                    token,
                    last ? last->word : "N/A",
                    pred ? pred->word : "N/A",
                    succ ? succ->word : "N/A");
            }
            token = strtok(NULL, " \t");
        }
    }

    free_tree(root);
    return 0;
}
