/*
 * generate_citizens.c
 * Διαβάζει ονόματα από names.txt και πολιτείες από states.txt,
 * δημιουργεί τυχαίες εγγραφές πολιτών και τις γράφει στο citizens.txt.
 * Κάθε πολίτης έχει: full_name, age [1-100], state, annual_income [0-1000000]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAMES 1000
#define MAX_STATES 50
#define NAME_LEN 100
#define STATE_LEN 50

int main(void) {
    char names[MAX_NAMES][NAME_LEN];
    char states[MAX_STATES][STATE_LEN];
    int name_count = 0, state_count = 0;
    FILE *fp;
    
    /* Ανάγνωση ονομάτων από names.txt */
    fp = fopen("names.txt", "r");
    if (!fp) {
        printf("Σφάλμα: δεν μπορεί να ανοίξει το names.txt\n");
        return 1;
    }
    while (name_count < MAX_NAMES && fgets(names[name_count], NAME_LEN, fp)) {
        /* Αφαίρεση χαρακτήρα νέας γραμμής */
        names[name_count][strcspn(names[name_count], "\r\n")] = '\0';
        if (strlen(names[name_count]) > 0)
            name_count++;
    }
    fclose(fp);

    /* Ανάγνωση πολιτειών από states.txt */
    fp = fopen("states.txt", "r");
    if (!fp) {
        printf("Σφάλμα: δεν μπορεί να ανοίξει το states.txt\n");
        return 1;
    }
    while (state_count < MAX_STATES && fgets(states[state_count], STATE_LEN, fp)) {
        states[state_count][strcspn(states[state_count], "\r\n")] = '\0';
        if (strlen(states[state_count]) > 0)
            state_count++;
    }
    fclose(fp);

    printf("Φορτώθηκαν %d ονόματα και %d πολιτείες\n", name_count, state_count);

    /* Δημιουργία citizens.txt */
    srand((unsigned int)time(NULL));
    fp = fopen("citizens.txt", "w");
    if (!fp) {
        printf("Σφάλμα: δεν μπορεί να δημιουργηθεί το citizens.txt\n");
        return 1;
    }

    for (int i = 0; i < name_count; i++) {
        int age = (rand() % 100) + 1;            /* [1, 100] */
        int income = rand() % 1000001;            /* [0, 1000000] */
        int state_idx = rand() % state_count;
        fprintf(fp, "%s,%d,%s,%d\n", names[i], age, states[state_idx], income);
    }

    fclose(fp);
    printf("Δημιουργήθηκαν %d εγγραφές πολιτών στο citizens.txt\n", name_count);
    return 0;
}
