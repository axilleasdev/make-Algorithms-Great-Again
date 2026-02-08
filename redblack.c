/*
 * rbt.c
 * Υλοποίηση Red-Black Δέντρου (αυτο-ισορροπούμενο δυαδικό δέντρο αναζήτησης)
 * Διατηρεί ισορροπία μέσω χρωματισμού κόμβων (Κόκκινο/Μαύρο) και περιστροφών
 *
 * Ιδιότητες Red-Black:
 * 1. Κάθε κόμβος είναι Κόκκινος ή Μαύρος
 * 2. Η ρίζα είναι πάντα Μαύρη
 * 3. Τα NULL φύλλα θεωρούνται Μαύρα
 * 4. Κόκκινος κόμβος δεν μπορεί να έχει κόκκινο παιδί
 * 5. Κάθε μονοπάτι από κόμβο σε NULL φύλλο έχει ίδιο αριθμό μαύρων κόμβων
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Δομή πολίτη */
typedef struct {
    char full_name[100];
    int age;
    char state[50];
    int annual_income;
} Citizen;

/* Δομή κόμβου δέντρου */
typedef struct Node {
    Citizen data;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
    int height;
    char color; /* 'R' = Κόκκινο, 'B' = Μαύρο */
} Node;

/* Φρουρός NIL κόμβος - αντικαθιστά τα NULL φύλλα */
static Node NIL_NODE = {{""}, NULL, NULL, NULL, 0, 'B'};
static Node *NIL = &NIL_NODE;

/* Δημιουργία νέου κόμβου (κόκκινος αρχικά) */
Node* create_node(Citizen data) {
    Node *node = (Node*)malloc(sizeof(Node));
    if (!node) {
        printf("Σφάλμα: αποτυχία δέσμευσης μνήμης\n");
        return NULL;
    }
    node->data = data;
    node->left = NIL;
    node->right = NIL;
    node->parent = NIL;
    node->height = 1;
    node->color = 'R'; /* Νέος κόμβος πάντα κόκκινος */
    return node;
}

/* Αριστερή περιστροφή γύρω από τον κόμβο x */
Node* rotate_left(Node *root, Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NIL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
    return root;
}

/* Δεξιά περιστροφή γύρω από τον κόμβο y */
Node* rotate_right(Node *root, Node *y) {
    Node *x = y->left;
    y->left = x->right;
    if (x->right != NIL)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NIL)
        root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
    return root;
}

/* Διόρθωση ιδιοτήτων Red-Black μετά από εισαγωγή */
Node* insert_fixup(Node *root, Node *z) {
    while (z->parent->color == 'R') {
        if (z->parent == z->parent->parent->left) {
            Node *uncle = z->parent->parent->right;
            if (uncle->color == 'R') {
                /* Περίπτωση 1: Ο θείος είναι κόκκινος - αναχρωματισμός */
                z->parent->color = 'B';
                uncle->color = 'B';
                z->parent->parent->color = 'R';
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    /* Περίπτωση 2: Ο θείος μαύρος, z δεξί παιδί */
                    z = z->parent;
                    root = rotate_left(root, z);
                }
                /* Περίπτωση 3: Ο θείος μαύρος, z αριστερό παιδί */
                z->parent->color = 'B';
                z->parent->parent->color = 'R';
                root = rotate_right(root, z->parent->parent);
            }
        } else {
            /* Συμμετρικές περιπτώσεις (γονέας = δεξί παιδί του παππού) */
            Node *uncle = z->parent->parent->left;
            if (uncle->color == 'R') {
                z->parent->color = 'B';
                uncle->color = 'B';
                z->parent->parent->color = 'R';
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    root = rotate_right(root, z);
                }
                z->parent->color = 'B';
                z->parent->parent->color = 'R';
                root = rotate_left(root, z->parent->parent);
            }
        }
    }
    root->color = 'B'; /* Η ρίζα πάντα μαύρη */
    return root;
}

/* Εισαγωγή κόμβου στο Red-Black δέντρο */
Node* insert_rbt(Node *root, Citizen data) {
    Node *z = create_node(data);
    Node *y = NIL;
    Node *x = (root == NULL) ? NIL : root;

    /* Εύρεση θέσης εισαγωγής (όπως BST) */
    while (x != NIL) {
        y = x;
        int cmp = strcmp(data.full_name, x->data.full_name);
        if (cmp < 0)
            x = x->left;
        else if (cmp > 0)
            x = x->right;
        else {
            free(z); /* Διπλότυπο - δεν εισάγεται */
            return root;
        }
    }

    z->parent = y;
    if (y == NIL)
        root = z;
    else if (strcmp(data.full_name, y->data.full_name) < 0)
        y->left = z;
    else
        y->right = z;

    /* Διόρθωση ιδιοτήτων */
    return insert_fixup(root, z);
}

/* Αντικατάσταση υποδέντρου u με υποδέντρο v */
Node* transplant(Node *root, Node *u, Node *v) {
    if (u->parent == NIL)
        root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
    return root;
}

/* Εύρεση ελάχιστου κόμβου */
Node* find_min(Node *root) {
    while (root->left != NIL)
        root = root->left;
    return root;
}

/* Διόρθωση ιδιοτήτων Red-Black μετά από διαγραφή */
Node* delete_fixup(Node *root, Node *x) {
    while (x != root && x->color == 'B') {
        if (x == x->parent->left) {
            Node *w = x->parent->right;
            if (w->color == 'R') {
                /* Περίπτωση 1: Ο αδελφός w είναι κόκκινος */
                w->color = 'B';
                x->parent->color = 'R';
                root = rotate_left(root, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == 'B' && w->right->color == 'B') {
                /* Περίπτωση 2: Και τα δύο παιδιά του w μαύρα */
                w->color = 'R';
                x = x->parent;
            } else {
                if (w->right->color == 'B') {
                    /* Περίπτωση 3: Δεξί παιδί του w μαύρο */
                    w->left->color = 'B';
                    w->color = 'R';
                    root = rotate_right(root, w);
                    w = x->parent->right;
                }
                /* Περίπτωση 4: Δεξί παιδί του w κόκκινο */
                w->color = x->parent->color;
                x->parent->color = 'B';
                w->right->color = 'B';
                root = rotate_left(root, x->parent);
                x = root;
            }
        } else {
            /* Συμμετρικές περιπτώσεις */
            Node *w = x->parent->left;
            if (w->color == 'R') {
                w->color = 'B';
                x->parent->color = 'R';
                root = rotate_right(root, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == 'B' && w->left->color == 'B') {
                w->color = 'R';
                x = x->parent;
            } else {
                if (w->left->color == 'B') {
                    w->right->color = 'B';
                    w->color = 'R';
                    root = rotate_left(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 'B';
                w->left->color = 'B';
                root = rotate_right(root, x->parent);
                x = root;
            }
        }
    }
    x->color = 'B';
    return root;
}

/* Διαγραφή κόμβου από το Red-Black δέντρο */
Node* delete_rbt(Node *root, char *name) {
    /* Εύρεση κόμβου */
    Node *z = (root == NULL) ? NIL : root;
    while (z != NIL) {
        int cmp = strcmp(name, z->data.full_name);
        if (cmp == 0) break;
        z = (cmp < 0) ? z->left : z->right;
    }
    if (z == NIL)
        return root; /* Δεν βρέθηκε */

    Node *y = z;
    Node *x;
    char y_original_color = y->color;

    if (z->left == NIL) {
        x = z->right;
        root = transplant(root, z, z->right);
    } else if (z->right == NIL) {
        x = z->left;
        root = transplant(root, z, z->left);
    } else {
        /* Κόμβος με δύο παιδιά */
        y = find_min(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            root = transplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        root = transplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    free(z);

    /* Διόρθωση αν διαγράφηκε μαύρος κόμβος */
    if (y_original_color == 'B')
        root = delete_fixup(root, x);

    return root;
}

/* Αναζήτηση κόμβου με βάση το όνομα */
Node* search(Node *root, char *name) {
    if (root == NULL || root == NIL)
        return NULL;

    int cmp = strcmp(name, root->data.full_name);
    if (cmp == 0)
        return root;
    else if (cmp < 0)
        return search(root->left, name);
    else
        return search(root->right, name);
}

/* Inorder traversal - εκτύπωση σε αλφαβητική σειρά */
void traversal(Node *root) {
    if (root == NULL || root == NIL)
        return;
    traversal(root->left);
    printf("%s,%d,%s,%d\n", root->data.full_name, root->data.age,
           root->data.state, root->data.annual_income);
    traversal(root->right);
}

/* Inorder traversal - εγγραφή σε αρχείο */
void traversal_to_file(Node *root, FILE *fp) {
    if (root == NULL || root == NIL)
        return;
    traversal_to_file(root->left, fp);
    fprintf(fp, "%s,%d,%s,%d\n", root->data.full_name, root->data.age,
            root->data.state, root->data.annual_income);
    traversal_to_file(root->right, fp);
}

/* Απελευθέρωση μνήμης δέντρου */
void free_tree(Node *root) {
    if (root == NULL || root == NIL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
