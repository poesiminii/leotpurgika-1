#ifndef IPC_UTILS_H
#define IPC_UTILS_H

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

// Δομή για δεδομένα της λέμβου
typedef struct {
    int capacity;            // Μέγιστη χωρητικότητα της λέμβου
    int passengers_onboard;  // Επιβάτες που έχουν ήδη επιβιβαστεί
    sem_t boarding_sem;      // Σημαφόρος για τις θέσεις στη λέμβο
    sem_t mutex;             // Δυαδικός σημαφόρος για την προστασία των μεταβλητών
    sem_t ready_to_depart;   // Σημαφόρος για αναχώρηση όταν η λέμβος γεμίσει
} Lifeboat;

// Αρχικοποίηση λέμβου
static inline void lifeboat_init(Lifeboat *lifeboat, int capacity) {
    lifeboat->capacity = capacity;
    lifeboat->passengers_onboard = 0;
    sem_init(&lifeboat->boarding_sem, 0, capacity); // Σημαφόρος με μέγιστη χωρητικότητα
    sem_init(&lifeboat->mutex, 0, 1);              // Δυαδικός σημαφόρος για συγχρονισμό
    sem_init(&lifeboat->ready_to_depart, 0, 0);    // Σημαφόρος για αναχώρηση
}

// Καταστροφή σημαφόρων της λέμβου
static inline void lifeboat_destroy(Lifeboat *lifeboat) {
    sem_destroy(&lifeboat->boarding_sem);
    sem_destroy(&lifeboat->mutex);
    sem_destroy(&lifeboat->ready_to_depart);
}

// Επιβίβαση επιβάτη
static inline void board_lifeboat(Lifeboat *lifeboat) {
    sem_wait(&lifeboat->boarding_sem); // Περιμένει διαθέσιμη θέση
    sem_wait(&lifeboat->mutex);        // Κλείδωμα για κρίσιμες μεταβλητές

    lifeboat->passengers_onboard++;
    printf("Επιβάτης επιβιβάστηκε. Επιβάτες στη λέμβο: %d/%d\n",
           lifeboat->passengers_onboard, lifeboat->capacity);

    if (lifeboat->passengers_onboard == lifeboat->capacity) {
        sem_post(&lifeboat->ready_to_depart); // Ενημέρωση για αναχώρηση
    }

    sem_post(&lifeboat->mutex);        // Ξεκλείδωμα
}

// Αποβίβαση επιβάτη
static inline void disembark_lifeboat(Lifeboat *lifeboat) {
    sem_wait(&lifeboat->mutex);        // Κλείδωμα για κρίσιμες μεταβλητές

    lifeboat->passengers_onboard--;
    printf("Επιβάτης αποβιβάστηκε. Επιβάτες στη λέμβο: %d/%d\n",
           lifeboat->passengers_onboard, lifeboat->capacity);

    sem_post(&lifeboat->mutex);        // Ξεκλείδωμα
    sem_post(&lifeboat->boarding_sem); // Απελευθέρωση θέσης
}

#endif // IPC_UTILS_H
