#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Function prototype
void binary_concurrent(int *vet, int n);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error: Usage %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int *vet = (int *)malloc(n * sizeof(int));
    
    if (vet == NULL) {
        printf("Allocation Error.\n");
        exit(1);
    }

    // Force terminal to print immediately (Quer's golden rule!)
    setbuf(stdout, 0); 

    printf("Binary Numbers:\n");
    
    // Call our new concurrent function
    binary_concurrent(vet, n);

    // The parent process should wait for all children to finish before ending
    // Wait returns -1 when there are no more children to wait for.
    while(wait(NULL) > 0); 

    free(vet);
    return 0;
}

void binary_concurrent(int *vet, int n) {
    pid_t pid;

    // Loop through each bit position
    for (int i = 0; i < n; i++) {
        pid = fork();

        if (pid > 0) {
            // I AM THE PARENT: I take the '0' path
            vet[i] = 0;
        } else if (pid == 0) {
            // I AM THE CHILD: I take the '1' path
            vet[i] = 1;
        } else {
            printf("Fork failed!\n");
            exit(1);
        }
        // Both Parent and Child automatically continue to the next loop iteration!
    }

    // --- BASE CASE (The End of the Loop) ---
    // If a process makes it here, it has filled all 'n' spots in its array.
    for (int j = 0; j < n; j++) {
        printf("%d", vet[j]);
    }
    printf("\n");

    // CRITICAL STEP: The process must die after printing!
    // If we don't exit here, 2^n processes will return to main and try to free(vet).
    exit(0); 
}