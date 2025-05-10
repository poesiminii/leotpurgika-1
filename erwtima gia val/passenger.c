#include "ipc_utils.h"
#include <unistd.h>
#include <stdio.h>

static void passenger(Lifeboat *lifeboat) {
    printf("Επιβάτης περιμένει να επιβιβαστεί...\n");
    board_lifeboat(lifeboat); // Επιβίβαση στη λέμβο
    printf("Επιβάτης είναι στη λέμβο.\n");
}
