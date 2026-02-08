/*
 * benchmark.c
 * Μέτρηση και σύγκριση απόδοσης BST, AVL και Red-Black δέντρων
 * Εκτελεί insert, search, delete και υπολογίζει μέσο χρόνο σε 5 επαναλήψεις
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* Επιστρέφει τρέχοντα χρόνο σε microseconds */
double get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

/* Συμπερίληψη υλοποιήσεων - κάθε αρχείο ορίζει τις δικές του συναρτήσεις */
/* Χρησιμοποιούμε ξεχωριστά prefixes για αποφυγή συγκρούσεων ονομάτων */

/* ============ Δομές Δεδομένων ============ */

typedef struct {
    char full_name[100];
    int age;
    char state[50];
    int annual_income;
} Citizen;

/* Κόμβος BST/AVL */
typedef struct BSTNode {
    Citizen data;
    struct BSTNode *left;
    struct BSTNode *right;
    int height;
} BSTNode;

/* Κόμβος Red-Black */
typedef struct RBTNode {
    Citizen data;
    struct RBTNode *left;
    struct RBTNode *right;
    struct RBTNode *parent;
    int height;
    char color;
} RBTNode;

/* ============ BST Συναρτήσεις ============ */

BSTNode* bst_create(Citizen data) {
    BSTNode *n = (BSTNode*)malloc(sizeof(BSTNode));
    n->data = data;
    n->left = n->right = NULL;
    n->height = 1;
    return n;
}

BSTNode* bst_insert(BSTNode *root, Citizen data) {
    if (!root) return bst_create(data);
    int cmp = strcmp(data.full_name, root->data.full_name);
    if (cmp < 0) root->left = bst_insert(root->left, data);
    else if (cmp > 0) root->right = bst_insert(root->right, data);
    return root;
}

BSTNode* bst_find_min(BSTNode *root) {
    while (root && root->left) root = root->left;
    return root;
}

BSTNode* bst_delete(BSTNode *root, char *name) {
    if (!root) return NULL;
    int cmp = strcmp(name, root->data.full_name);
    if (cmp < 0) root->left = bst_delete(root->left, name);
    else if (cmp > 0) root->right = bst_delete(root->right, name);
    else {
        if (!root->left) { BSTNode *t = root->right; free(root); return t; }
        if (!root->right) { BSTNode *t = root->left; free(root); return t; }
        BSTNode *s = bst_find_min(root->right);
        root->data = s->data;
        root->right = bst_delete(root->right, s->data.full_name);
    }
    return root;
}

BSTNode* bst_search(BSTNode *root, char *name) {
    if (!root) return NULL;
    int cmp = strcmp(name, root->data.full_name);
    if (cmp == 0) return root;
    return (cmp < 0) ? bst_search(root->left, name) : bst_search(root->right, name);
}

void bst_free(BSTNode *root) {
    if (!root) return;
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

/* ============ AVL Συναρτήσεις ============ */

int avl_height(BSTNode *n) { return n ? n->height : 0; }
int avl_max(int a, int b) { return a > b ? a : b; }
int avl_balance(BSTNode *n) { return n ? avl_height(n->left) - avl_height(n->right) : 0; }

BSTNode* avl_rotate_right(BSTNode *y) {
    BSTNode *x = y->left, *T = x->right;
    x->right = y; y->left = T;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    return x;
}

BSTNode* avl_rotate_left(BSTNode *x) {
    BSTNode *y = x->right, *T = y->left;
    y->left = x; x->right = T;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    return y;
}

BSTNode* avl_insert(BSTNode *root, Citizen data) {
    if (!root) return bst_create(data);
    int cmp = strcmp(data.full_name, root->data.full_name);
    if (cmp < 0) root->left = avl_insert(root->left, data);
    else if (cmp > 0) root->right = avl_insert(root->right, data);
    else return root;

    root->height = avl_max(avl_height(root->left), avl_height(root->right)) + 1;
    int bal = avl_balance(root);

    if (bal > 1 && strcmp(data.full_name, root->left->data.full_name) < 0)
        return avl_rotate_right(root);
    if (bal < -1 && strcmp(data.full_name, root->right->data.full_name) > 0)
        return avl_rotate_left(root);
    if (bal > 1 && strcmp(data.full_name, root->left->data.full_name) > 0) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }
    if (bal < -1 && strcmp(data.full_name, root->right->data.full_name) < 0) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }
    return root;
}

BSTNode* avl_delete(BSTNode *root, char *name) {
    if (!root) return NULL;
    int cmp = strcmp(name, root->data.full_name);
    if (cmp < 0) root->left = avl_delete(root->left, name);
    else if (cmp > 0) root->right = avl_delete(root->right, name);
    else {
        if (!root->left || !root->right) {
            BSTNode *t = root->left ? root->left : root->right;
            if (!t) { t = root; root = NULL; }
            else { *root = *t; }
            free(t);
        } else {
            BSTNode *s = bst_find_min(root->right);
            root->data = s->data;
            root->right = avl_delete(root->right, s->data.full_name);
        }
    }
    if (!root) return NULL;

    root->height = avl_max(avl_height(root->left), avl_height(root->right)) + 1;
    int bal = avl_balance(root);

    if (bal > 1 && avl_balance(root->left) >= 0) return avl_rotate_right(root);
    if (bal > 1 && avl_balance(root->left) < 0) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }
    if (bal < -1 && avl_balance(root->right) <= 0) return avl_rotate_left(root);
    if (bal < -1 && avl_balance(root->right) > 0) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }
    return root;
}

/* ============ Red-Black Συναρτήσεις ============ */

static RBTNode RBT_NIL_NODE = {{""}, NULL, NULL, NULL, 0, 'B'};
static RBTNode *RBT_NIL = &RBT_NIL_NODE;

RBTNode* rbt_create(Citizen data) {
    RBTNode *n = (RBTNode*)malloc(sizeof(RBTNode));
    n->data = data;
    n->left = n->right = RBT_NIL;
    n->parent = RBT_NIL;
    n->height = 1;
    n->color = 'R';
    return n;
}

RBTNode* rbt_rotate_left(RBTNode *root, RBTNode *x) {
    RBTNode *y = x->right;
    x->right = y->left;
    if (y->left != RBT_NIL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == RBT_NIL) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x; x->parent = y;
    return root;
}

RBTNode* rbt_rotate_right(RBTNode *root, RBTNode *y) {
    RBTNode *x = y->left;
    y->left = x->right;
    if (x->right != RBT_NIL) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == RBT_NIL) root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;
    x->right = y; y->parent = x;
    return root;
}

RBTNode* rbt_insert_fixup(RBTNode *root, RBTNode *z) {
    while (z->parent->color == 'R') {
        if (z->parent == z->parent->parent->left) {
            RBTNode *u = z->parent->parent->right;
            if (u->color == 'R') {
                z->parent->color = 'B'; u->color = 'B';
                z->parent->parent->color = 'R'; z = z->parent->parent;
            } else {
                if (z == z->parent->right) { z = z->parent; root = rbt_rotate_left(root, z); }
                z->parent->color = 'B'; z->parent->parent->color = 'R';
                root = rbt_rotate_right(root, z->parent->parent);
            }
        } else {
            RBTNode *u = z->parent->parent->left;
            if (u->color == 'R') {
                z->parent->color = 'B'; u->color = 'B';
                z->parent->parent->color = 'R'; z = z->parent->parent;
            } else {
                if (z == z->parent->left) { z = z->parent; root = rbt_rotate_right(root, z); }
                z->parent->color = 'B'; z->parent->parent->color = 'R';
                root = rbt_rotate_left(root, z->parent->parent);
            }
        }
    }
    root->color = 'B';
    return root;
}

RBTNode* rbt_insert(RBTNode *root, Citizen data) {
    RBTNode *z = rbt_create(data);
    RBTNode *y = RBT_NIL, *x = root ? root : RBT_NIL;
    while (x != RBT_NIL) {
        y = x;
        int cmp = strcmp(data.full_name, x->data.full_name);
        if (cmp < 0) x = x->left;
        else if (cmp > 0) x = x->right;
        else { free(z); return root; }
    }
    z->parent = y;
    if (y == RBT_NIL) root = z;
    else if (strcmp(data.full_name, y->data.full_name) < 0) y->left = z;
    else y->right = z;
    return rbt_insert_fixup(root, z);
}

RBTNode* rbt_transplant(RBTNode *root, RBTNode *u, RBTNode *v) {
    if (u->parent == RBT_NIL) root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
    return root;
}

RBTNode* rbt_find_min(RBTNode *root) {
    while (root->left != RBT_NIL) root = root->left;
    return root;
}

RBTNode* rbt_delete_fixup(RBTNode *root, RBTNode *x) {
    while (x != root && x->color == 'B') {
        if (x == x->parent->left) {
            RBTNode *w = x->parent->right;
            if (w->color == 'R') {
                w->color = 'B'; x->parent->color = 'R';
                root = rbt_rotate_left(root, x->parent); w = x->parent->right;
            }
            if (w->left->color == 'B' && w->right->color == 'B') {
                w->color = 'R'; x = x->parent;
            } else {
                if (w->right->color == 'B') {
                    w->left->color = 'B'; w->color = 'R';
                    root = rbt_rotate_right(root, w); w = x->parent->right;
                }
                w->color = x->parent->color; x->parent->color = 'B';
                w->right->color = 'B'; root = rbt_rotate_left(root, x->parent);
                x = root;
            }
        } else {
            RBTNode *w = x->parent->left;
            if (w->color == 'R') {
                w->color = 'B'; x->parent->color = 'R';
                root = rbt_rotate_right(root, x->parent); w = x->parent->left;
            }
            if (w->right->color == 'B' && w->left->color == 'B') {
                w->color = 'R'; x = x->parent;
            } else {
                if (w->left->color == 'B') {
                    w->right->color = 'B'; w->color = 'R';
                    root = rbt_rotate_left(root, w); w = x->parent->left;
                }
                w->color = x->parent->color; x->parent->color = 'B';
                w->left->color = 'B'; root = rbt_rotate_right(root, x->parent);
                x = root;
            }
        }
    }
    x->color = 'B';
    return root;
}

RBTNode* rbt_delete(RBTNode *root, char *name) {
    RBTNode *z = root ? root : RBT_NIL;
    while (z != RBT_NIL) {
        int cmp = strcmp(name, z->data.full_name);
        if (cmp == 0) break;
        z = (cmp < 0) ? z->left : z->right;
    }
    if (z == RBT_NIL) return root;

    RBTNode *y = z, *x;
    char orig = y->color;
    if (z->left == RBT_NIL) { x = z->right; root = rbt_transplant(root, z, z->right); }
    else if (z->right == RBT_NIL) { x = z->left; root = rbt_transplant(root, z, z->left); }
    else {
        y = rbt_find_min(z->right); orig = y->color; x = y->right;
        if (y->parent == z) { x->parent = y; }
        else { root = rbt_transplant(root, y, y->right); y->right = z->right; y->right->parent = y; }
        root = rbt_transplant(root, z, y); y->left = z->left; y->left->parent = y; y->color = z->color;
    }
    free(z);
    if (orig == 'B') root = rbt_delete_fixup(root, x);
    return root;
}

RBTNode* rbt_search(RBTNode *root, char *name) {
    if (!root || root == RBT_NIL) return NULL;
    int cmp = strcmp(name, root->data.full_name);
    if (cmp == 0) return root;
    return (cmp < 0) ? rbt_search(root->left, name) : rbt_search(root->right, name);
}

void rbt_free(RBTNode *root) {
    if (!root || root == RBT_NIL) return;
    rbt_free(root->left);
    rbt_free(root->right);
    free(root);
}

/* ============ Φόρτωση Δεδομένων ============ */

#define MAX_CITIZENS 1000

/* Ανάγνωση εγγραφών πολιτών από citizens.txt */
int load_citizens(Citizen citizens[], int max) {
    FILE *fp = fopen("citizens.txt", "r");
    if (!fp) { printf("Σφάλμα: δεν ανοίγει το citizens.txt\n"); return 0; }
    int count = 0;
    char line[256];
    while (count < max && fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        /* Ανάλυση γραμμής: όνομα,ηλικία,πολιτεία,εισόδημα */
        char *token = strtok(line, ",");
        if (!token) continue;
        strncpy(citizens[count].full_name, token, 99);
        token = strtok(NULL, ",");
        if (token) citizens[count].age = atoi(token);
        token = strtok(NULL, ",");
        if (token) strncpy(citizens[count].state, token, 49);
        token = strtok(NULL, ",");
        if (token) citizens[count].annual_income = atoi(token);
        count++;
    }
    fclose(fp);
    return count;
}

/* ============ Κύριο Πρόγραμμα Benchmark ============ */

#define RUNS 5
#define SEARCH_DELETE_COUNT 100

int main(void) {
    Citizen citizens[MAX_CITIZENS];
    int count = load_citizens(citizens, MAX_CITIZENS);
    if (count == 0) return 1;
    printf("Φορτώθηκαν %d εγγραφές πολιτών\n\n", count);

    /* Μεταβλητές χρόνου */
    double bst_insert_time = 0, bst_search_time = 0, bst_delete_time = 0;
    double avl_insert_time = 0, avl_search_time = 0, avl_delete_time = 0;
    double rbt_insert_time = 0, rbt_search_time = 0, rbt_delete_time = 0;

    srand((unsigned int)time(NULL));

    /* Επιλογή 100 τυχαίων ονομάτων για search και delete */
    int indices[SEARCH_DELETE_COUNT];
    for (int i = 0; i < SEARCH_DELETE_COUNT; i++)
        indices[i] = rand() % count;

    /* Εκτέλεση 5 επαναλήψεων */
    for (int run = 0; run < RUNS; run++) {
        double start, end;
        BSTNode *bst_root = NULL;
        BSTNode *avl_root = NULL;
        RBTNode *rbt_root = NULL;

        printf("Επανάληψη %d/%d...\n", run + 1, RUNS);

        /* --- BST Insert --- */
        start = get_time_us();
        for (int i = 0; i < count; i++)
            bst_root = bst_insert(bst_root, citizens[i]);
        end = get_time_us();
        bst_insert_time += (end - start);

        /* --- AVL Insert --- */
        start = get_time_us();
        for (int i = 0; i < count; i++)
            avl_root = avl_insert(avl_root, citizens[i]);
        end = get_time_us();
        avl_insert_time += (end - start);

        /* --- RBT Insert --- */
        start = get_time_us();
        for (int i = 0; i < count; i++)
            rbt_root = rbt_insert(rbt_root, citizens[i]);
        end = get_time_us();
        rbt_insert_time += (end - start);

        /* --- BST Search --- */
        start = get_time_us();
        for (int i = 0; i < SEARCH_DELETE_COUNT; i++)
            bst_search(bst_root, citizens[indices[i]].full_name);
        end = get_time_us();
        bst_search_time += (end - start);

        /* --- AVL Search --- */
        start = get_time_us();
        for (int i = 0; i < SEARCH_DELETE_COUNT; i++)
            bst_search(avl_root, citizens[indices[i]].full_name);
        end = get_time_us();
        avl_search_time += (end - start);

        /* --- RBT Search --- */
        start = get_time_us();
        for (int i = 0; i < SEARCH_DELETE_COUNT; i++)
            rbt_search(rbt_root, citizens[indices[i]].full_name);
        end = get_time_us();
        rbt_search_time += (end - start);

        /* --- BST Delete --- */
        start = get_time_us();
        for (int i = 0; i < SEARCH_DELETE_COUNT; i++)
            bst_root = bst_delete(bst_root, citizens[indices[i]].full_name);
        end = get_time_us();
        bst_delete_time += (end - start);

        /* --- AVL Delete --- */
        start = get_time_us();
        for (int i = 0; i < SEARCH_DELETE_COUNT; i++)
            avl_root = avl_delete(avl_root, citizens[indices[i]].full_name);
        end = get_time_us();
        avl_delete_time += (end - start);

        /* --- RBT Delete --- */
        start = get_time_us();
        for (int i = 0; i < SEARCH_DELETE_COUNT; i++)
            rbt_root = rbt_delete(rbt_root, citizens[indices[i]].full_name);
        end = get_time_us();
        rbt_delete_time += (end - start);

        /* Απελευθέρωση μνήμης */
        bst_free(bst_root);
        bst_free(avl_root);
        rbt_free(rbt_root);
    }

    /* Υπολογισμός μέσου όρου (ήδη σε microseconds) */
    bst_insert_time /= RUNS; bst_search_time /= RUNS; bst_delete_time /= RUNS;
    avl_insert_time /= RUNS; avl_search_time /= RUNS; avl_delete_time /= RUNS;
    rbt_insert_time /= RUNS; rbt_search_time /= RUNS; rbt_delete_time /= RUNS;

    /* Εκτύπωση αποτελεσμάτων */
    printf("\n========================================\n");
    printf("       ΑΠΟΤΕΛΕΣΜΑΤΑ BENCHMARK\n");
    printf("   (Μέσος όρος %d επαναλήψεων)\n", RUNS);
    printf("========================================\n");
    printf("%-12s %12s %12s %12s\n", "Operation", "BST (us)", "AVL (us)", "RBT (us)");
    printf("----------------------------------------\n");
    printf("%-12s %12.2f %12.2f %12.2f\n", "Insert", bst_insert_time, avl_insert_time, rbt_insert_time);
    printf("%-12s %12.2f %12.2f %12.2f\n", "Search", bst_search_time, avl_search_time, rbt_search_time);
    printf("%-12s %12.2f %12.2f %12.2f\n", "Delete", bst_delete_time, avl_delete_time, rbt_delete_time);
    printf("========================================\n");

    /* Εγγραφή αποτελεσμάτων σε results.txt */
    FILE *fp = fopen("results.txt", "w");
    if (fp) {
        fprintf(fp, "Benchmark Results (Average of %d runs)\n", RUNS);
        fprintf(fp, "Insert: %d records | Search: %d names | Delete: %d names\n\n", count, SEARCH_DELETE_COUNT, SEARCH_DELETE_COUNT);
        fprintf(fp, "%-12s %12s %12s %12s\n", "Operation", "BST (us)", "AVL (us)", "RBT (us)");
        fprintf(fp, "----------------------------------------------------\n");
        fprintf(fp, "%-12s %12.2f %12.2f %12.2f\n", "Insert", bst_insert_time, avl_insert_time, rbt_insert_time);
        fprintf(fp, "%-12s %12.2f %12.2f %12.2f\n", "Search", bst_search_time, avl_search_time, rbt_search_time);
        fprintf(fp, "%-12s %12.2f %12.2f %12.2f\n", "Delete", bst_delete_time, avl_delete_time, rbt_delete_time);
        fclose(fp);
        printf("\nΤα αποτελέσματα αποθηκεύτηκαν στο results.txt\n");
    }

    return 0;
}
