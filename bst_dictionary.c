/*
 * Plain BST Dictionary - Spell Checker (comparison baseline)
 * ==========================================================
 * Same program as dictionary.c, but WITHOUT any balancing.
 * It exists to compare tree height against the AVL version
 * for different insertion orders (see README).
 *
 * Usage: ./bst_dictionary [wordlist]     (default: Dictionary.txt)
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
    struct Node *left, *right;
} Node;

/* ── BST helpers ── */

static int max2(int a, int b) {
    return a > b ? a : b;
}

static Node *new_node(const char *word) {
    Node *n = malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }
    strncpy(n->word, word, MAX_WORD - 1);
    n->word[MAX_WORD - 1] = '\0';
    n->left = n->right = NULL;
    return n;
}

/* Height of the tree (recursive; nodes do not store their height here) */
static int tree_height(Node *n) {
    return n ? 1 + max2(tree_height(n->left), tree_height(n->right)) : 0;
}

/* ── Core operations ── */

Node *insert(Node *root, const char *word) {
    if (!root) return new_node(word);
    int cmp = strcasecmp(word, root->word);
    if      (cmp < 0) root->left  = insert(root->left,  word);
    else if (cmp > 0) root->right = insert(root->right, word);
    else              return root;   /* duplicate – ignore */
    return root;
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


int main(int argc, char *argv[]) {
    /* Optional argument: path to a word list (default: Dictionary.txt) */
    const char *file = (argc > 1) ? argv[1] : "Dictionary.txt";
    Node *root = load_dictionary(file);

    printf("Dictionary Loaded Successfully...!\n");
    printf(".........................\n");
    printf("Size = %d\n", tree_size(root));
    printf(".........................\n");
    printf("Height = %d\n", tree_height(root));
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
