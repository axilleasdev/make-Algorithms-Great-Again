/*
 * avl.c
 * Υλοποίηση AVL Δέντρου (αυτο-ισορροπούμενο δυαδικό δέντρο αναζήτησης)
 * Διατηρεί ισορροπία μέσω περιστροφών όταν ο balance factor υπερβεί το ±1
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
    int height;
    char color;
} Node;

/* Επιστρέφει το ύψος ενός κόμβου */
int height(Node *n) {
    return n ? n->height : 0;
}

/* Επιστρέφει το μεγαλύτερο από δύο ακέραιους */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/* Υπολογισμός balance factor: ύψος αριστερού - ύψος δεξιού υποδέντρου */
int get_balance(Node *n) {
    return n ? height(n->left) - height(n->right) : 0;
}

/* Δημιουργία νέου κόμβου */
Node* create_node(Citizen data) {
    Node *node = (Node*)malloc(sizeof(Node));
    if (!node) {
        printf("Σφάλμα: αποτυχία δέσμευσης μνήμης\n");
        return NULL;
    }
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    node->color = 'R';
    return node;
}

/* Δεξιά περιστροφή γύρω από τον κόμβο y */
Node* rotate_right(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;

    x->right = y;
    y->left = T2;

    /* Ενημέρωση υψών */
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

/* Αριστερή περιστροφή γύρω από τον κόμβο x */
Node* rotate_left(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;

    y->left = x;
    x->right = T2;

    /* Ενημέρωση υψών */
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

/* Εισαγωγή κόμβου στο AVL δέντρο */
Node* insert_avl(Node *root, Citizen data) {
    /* Κανονική εισαγωγή BST */
    if (root == NULL)
        return create_node(data);

    int cmp = strcmp(data.full_name, root->data.full_name);
    if (cmp < 0)
        root->left = insert_avl(root->left, data);
    else if (cmp > 0)
        root->right = insert_avl(root->right, data);
    else
        return root; /* Διπλότυπο - δεν εισάγεται */

    /* Ενημέρωση ύψους */
    root->height = max(height(root->left), height(root->right)) + 1;

    /* Έλεγχος ισορροπίας και περιστροφές */
    int balance = get_balance(root);

    /* Περίπτωση Αριστερά-Αριστερά (Left-Left) */
    if (balance > 1 && strcmp(data.full_name, root->left->data.full_name) < 0)
        return rotate_right(root);

    /* Περίπτωση Δεξιά-Δεξιά (Right-Right) */
    if (balance < -1 && strcmp(data.full_name, root->right->data.full_name) > 0)
        return rotate_left(root);

    /* Περίπτωση Αριστερά-Δεξιά (Left-Right) */
    if (balance > 1 && strcmp(data.full_name, root->left->data.full_name) > 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    /* Περίπτωση Δεξιά-Αριστερά (Right-Left) */
    if (balance < -1 && strcmp(data.full_name, root->right->data.full_name) < 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}

/* Εύρεση κόμβου με το μικρότερο κλειδί */
Node* find_min(Node *root) {
    while (root && root->left)
        root = root->left;
    return root;
}

/* Διαγραφή κόμβου από το AVL δέντρο */
Node* delete_avl(Node *root, char *name) {
    if (root == NULL)
        return NULL;

    int cmp = strcmp(name, root->data.full_name);
    if (cmp < 0)
        root->left = delete_avl(root->left, name);
    else if (cmp > 0)
        root->right = delete_avl(root->right, name);
    else {
        /* Βρέθηκε ο κόμβος προς διαγραφή */
        if (root->left == NULL || root->right == NULL) {
            Node *temp = root->left ? root->left : root->right;
            if (temp == NULL) {
                /* Κόμβος χωρίς παιδιά */
                temp = root;
                root = NULL;
            } else {
                /* Κόμβος με ένα παιδί */
                *root = *temp;
            }
            free(temp);
        } else {
            /* Κόμβος με δύο παιδιά - αντικατάσταση με inorder successor */
            Node *successor = find_min(root->right);
            root->data = successor->data;
            root->right = delete_avl(root->right, successor->data.full_name);
        }
    }

    if (root == NULL)
        return NULL;

    /* Ενημέρωση ύψους */
    root->height = max(height(root->left), height(root->right)) + 1;

    /* Εξισορρόπηση */
    int balance = get_balance(root);

    /* Αριστερά-Αριστερά */
    if (balance > 1 && get_balance(root->left) >= 0)
        return rotate_right(root);

    /* Αριστερά-Δεξιά */
    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    /* Δεξιά-Δεξιά */
    if (balance < -1 && get_balance(root->right) <= 0)
        return rotate_left(root);

    /* Δεξιά-Αριστερά */
    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}

/* Αναζήτηση κόμβου με βάση το όνομα */
Node* search(Node *root, char *name) {
    if (root == NULL)
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
    if (root == NULL)
        return;
    traversal(root->left);
    printf("%s,%d,%s,%d\n", root->data.full_name, root->data.age,
           root->data.state, root->data.annual_income);
    traversal(root->right);
}

/* Inorder traversal - εγγραφή σε αρχείο */
void traversal_to_file(Node *root, FILE *fp) {
    if (root == NULL)
        return;
    traversal_to_file(root->left, fp);
    fprintf(fp, "%s,%d,%s,%d\n", root->data.full_name, root->data.age,
            root->data.state, root->data.annual_income);
    traversal_to_file(root->right, fp);
}

/* Απελευθέρωση μνήμης δέντρου */
void free_tree(Node *root) {
    if (root == NULL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
