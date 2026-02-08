/*
 * bst.c
 * Υλοποίηση Δυαδικού Δέντρου Αναζήτησης (Binary Search Tree)
 * Αποθηκεύει εγγραφές πολιτών (Citizen) με κλειδί το full_name
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
    int height;   /* Χρησιμοποιείται από AVL */
    char color;   /* Χρησιμοποιείται από Red-Black ('R' ή 'B') */
} Node;

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

/* Εισαγωγή κόμβου στο BST - σύγκριση με βάση το full_name */
Node* insert_bst(Node *root, Citizen data) {
    if (root == NULL)
        return create_node(data);

    int cmp = strcmp(data.full_name, root->data.full_name);
    if (cmp < 0)
        root->left = insert_bst(root->left, data);
    else if (cmp > 0)
        root->right = insert_bst(root->right, data);
    /* Αν cmp == 0, το όνομα υπάρχει ήδη - δεν εισάγεται */

    return root;
}

/* Εύρεση κόμβου με το μικρότερο κλειδί (αριστερότερος κόμβος) */
Node* find_min(Node *root) {
    while (root && root->left)
        root = root->left;
    return root;
}

/* Διαγραφή κόμβου από το BST με βάση το όνομα */
Node* delete_bst(Node *root, char *name) {
    if (root == NULL)
        return NULL;

    int cmp = strcmp(name, root->data.full_name);
    if (cmp < 0) {
        root->left = delete_bst(root->left, name);
    } else if (cmp > 0) {
        root->right = delete_bst(root->right, name);
    } else {
        /* Βρέθηκε ο κόμβος προς διαγραφή */
        if (root->left == NULL) {
            /* Κόμβος με 0 ή 1 παιδί (δεξί) */
            Node *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            /* Κόμβος με 1 παιδί (αριστερό) */
            Node *temp = root->left;
            free(root);
            return temp;
        }
        /* Κόμβος με 2 παιδιά - αντικατάσταση με τον inorder successor */
        Node *successor = find_min(root->right);
        root->data = successor->data;
        root->right = delete_bst(root->right, successor->data.full_name);
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

/* Inorder traversal - εκτύπωση πολιτών σε αλφαβητική σειρά */
void traversal(Node *root) {
    if (root == NULL)
        return;
    traversal(root->left);
    printf("%s,%d,%s,%d\n", root->data.full_name, root->data.age,
           root->data.state, root->data.annual_income);
    traversal(root->right);
}

/* Inorder traversal - εγγραφή σε αρχείο για επαλήθευση */
void traversal_to_file(Node *root, FILE *fp) {
    if (root == NULL)
        return;
    traversal_to_file(root->left, fp);
    fprintf(fp, "%s,%d,%s,%d\n", root->data.full_name, root->data.age,
            root->data.state, root->data.annual_income);
    traversal_to_file(root->right, fp);
}

/* Απελευθέρωση μνήμης όλου του δέντρου */
void free_tree(Node *root) {
    if (root == NULL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
