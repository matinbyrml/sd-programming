#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    // TIMELINE: START
    // Only ONE process exists right now (The Parent).
    printf("1. This is printed by the Parent ONLY.\n");

    // THE MAGIC MOMENT
    pid_t secret_number = fork(); 

    // TIMELINE: AFTER FORK
    // From this line onward, TWO processes are reading the code at the same time!
    
    if (secret_number == 0) {
        // Only the CHILD goes inside here, because its secret number is 0.
        printf("2. CHILD: I am the clone! I start working here.\n");
        sleep(2); // The child takes a nap
        printf("3. CHILD: I am done. Goodbye!\n");
        exit(0);  // The CHILD dies exactly here.
    } 
    else if (secret_number > 0) {
        // Only the PARENT goes inside here, because its secret number is positive.
        printf("2. PARENT: I am the original! I spawned a child.\n");
        printf("3. PARENT: I am done. Goodbye!\n");
        // The PARENT drops to the bottom and dies.
    }

    // TIMELINE: END
    return 0; 
}