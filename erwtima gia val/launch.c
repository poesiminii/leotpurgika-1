// launch.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "ipc_utils.h"
#include "passenger.c"  // Include passenger.c so you can use the passenger function

#define MAX_PASSENGERS 100

int passengers_served = 0;  // Global variable to track the number of passengers served
sem_t passengers_served_sem;  // Semaphore to synchronize access to passengers_served

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Χρήση: %s <συνολικοί επιβάτες> <λέμβοι> <χωρητικότητα λέμβου>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int total_passengers = atoi(argv[1]);  // Συνολικός αριθμός επιβατών
    int total_lifeboats = atoi(argv[2]);  // Συνολικός αριθμός λεμβών
    int lifeboat_capacity = atoi(argv[3]); // Χωρητικότητα κάθε λέμβου

    if (total_passengers > MAX_PASSENGERS) {
        fprintf(stderr, "Το μέγιστο πλήθος επιβατών είναι %d.\n", MAX_PASSENGERS);
        return EXIT_FAILURE;
    }

    // Δημιουργία λέμβων
    Lifeboat *lifeboats = malloc(total_lifeboats * sizeof(Lifeboat));
    for (int i = 0; i < total_lifeboats; i++) {
        lifeboat_init(&lifeboats[i], lifeboat_capacity);
    }

    // Αρχικοποίηση σημαφόρου για την προστασία της μεταβλητής passengers_served
    sem_init(&passengers_served_sem, 0, 1);  // Binary semaphore (1 means unlocked)

    while (passengers_served < total_passengers) {
        for (int i = 0; i < total_lifeboats; i++) {
            if (passengers_served >= total_passengers) break;

            printf("Λέμβος %d είναι έτοιμη για επιβίβαση.\n", i + 1);

            int pid = fork();
            if (pid == 0) {
                // Παιδί: Προσομοίωση επιβίβασης
                for (int j = 0; j < lifeboat_capacity && passengers_served < total_passengers; j++) {
                    passenger(&lifeboats[i]); // Κλήση της συνάρτησης passenger από το passenger.c
                    
                    // Χρησιμοποιούμε το σημαφόρο για συγχρονισμό της τροποποίησης της μεταβλητής
                    sem_wait(&passengers_served_sem);  // Κλείδωμα του σημαφόρου
                    passengers_served++;  // Αυξάνουμε το πλήθος των εξυπηρετημένων επιβατών
                    printf("pass served: %d , total: %d !!!!!!!!!!!\n", passengers_served, total_passengers);
                    sem_post(&passengers_served_sem);  // Ξεκλείδωμα του σημαφόρου
                }

                // Μόλις γεμίσει η λέμβος, αναχωρεί
                sem_wait(&lifeboats[i].ready_to_depart);
                printf("Λέμβος %d αναχωρεί για τη στεριά.\n", i + 1);

                // Προσομοίωση αποβίβασης
                for (int j = 0; j < lifeboat_capacity; j++) {
                    disembark_lifeboat(&lifeboats[i]);
                }

                printf("Λέμβος %d επιστρέφει για επιβίβαση.\n", i + 1);
                exit(0);
            } else {
                wait(NULL); // Πατέρας: Περιμένει το παιδί να τελειώσει
            }
        }
    }

    // Καθαρισμός πόρων
    //sem_destroy(&passengers_served_sem);  // Καταστρέφουμε το σημαφόρο

    for (int i = 0; i < total_lifeboats; i++) {
        lifeboat_destroy(&lifeboats[i]);
    }
    free(lifeboats);

    printf("Όλοι οι επιβάτες ολοκλήρωσαν τη διαδικασία.\n");
    return EXIT_SUCCESS;
}
