#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // The thread library

// 1. The "Backpack" (Struct)
// We pack this struct with the data each thread needs to know about its history.
typedef struct {
    int max_depth;       // The user input 'n'
    int current_level;   // How deep we currently are
    pthread_t *history;  // An array tracking the IDs of our ancestors
} ThreadData;

// 2. The Thread Function
// Every new thread will start executing this specific block of code.
// Threads require the function parameter to be a generic "void *" pointer.
void *tree_generation(void *arg) {
    // Unpack the backpack
    ThreadData *my_data = (ThreadData *)arg;
    
    // Add my own Thread ID to the history array at my current level
    my_data->history[my_data->current_level] = pthread_self();

    // Are we a leaf? (Have we reached the target depth 'n'?)
    if (my_data->current_level == my_data->max_depth) {
        // Yes! Print the generation tree exactly as Quer requested.
        printf("T=%lu --> ", (unsigned long)my_data->history[my_data->max_depth]);
        for (int i = 0; i < my_data->max_depth; i++) {
            printf("%lu ", (unsigned long)my_data->history[i]);
        }
        printf("\n");
        
        // Free the dynamically allocated memory before we leave to prevent memory leaks!
        free(my_data->history);
        free(my_data);
        pthread_exit(NULL); // The thread ends here
    }

    // --- We are NOT a leaf. We must create two children. ---
    pthread_t tid1, tid2;
    
    // Build a new backpack for Child 1
    ThreadData *child1_data = (ThreadData *)malloc(sizeof(ThreadData));
    child1_data->max_depth = my_data->max_depth;
    child1_data->current_level = my_data->current_level + 1;
    child1_data->history = (pthread_t *)malloc((my_data->max_depth + 1) * sizeof(pthread_t));
    
    // Copy the family history into Child 1's backpack
    for (int i = 0; i <= my_data->current_level; i++) {
        child1_data->history[i] = my_data->history[i];
    }

    // Build a new backpack for Child 2 (Exact same process)
    ThreadData *child2_data = (ThreadData *)malloc(sizeof(ThreadData));
    child2_data->max_depth = my_data->max_depth;
    child2_data->current_level = my_data->current_level + 1;
    child2_data->history = (pthread_t *)malloc((my_data->max_depth + 1) * sizeof(pthread_t));
    
    for (int i = 0; i <= my_data->current_level; i++) {
        child2_data->history[i] = my_data->history[i];
    }

    // Create the two child threads, handing them their specific backpacks
    pthread_create(&tid1, NULL, tree_generation, (void *)child1_data);
    pthread_create(&tid2, NULL, tree_generation, (void *)child2_data);

    // The parent thread MUST wait for its two children to finish 
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // Clean up our own memory and exit
    free(my_data->history);
    free(my_data);
    pthread_exit(NULL);
}

// 3. The Main Program
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error: Usage %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    // The Main thread acts as the ultimate parent. 
    // We build the very first backpack for it.
    ThreadData *root_data = (ThreadData *)malloc(sizeof(ThreadData));
    root_data->max_depth = n;
    root_data->current_level = 0;
    root_data->history = (pthread_t *)malloc((n + 1) * sizeof(pthread_t));
    
    // We record the main thread's ID in the first slot
    root_data->history[0] = pthread_self();

    // Create the first two threads as requested by the text 
    pthread_t tid1, tid2;
    
    // We need to copy the root data for the first two children just like we did above
    ThreadData *child1_data = (ThreadData *)malloc(sizeof(ThreadData));
    child1_data->max_depth = n;
    child1_data->current_level = 1;
    child1_data->history = (pthread_t *)malloc((n + 1) * sizeof(pthread_t));
    child1_data->history[0] = root_data->history[0];

    ThreadData *child2_data = (ThreadData *)malloc(sizeof(ThreadData));
    child2_data->max_depth = n;
    child2_data->current_level = 1;
    child2_data->history = (pthread_t *)malloc((n + 1) * sizeof(pthread_t));
    child2_data->history[0] = root_data->history[0];

    pthread_create(&tid1, NULL, tree_generation, (void *)child1_data);
    pthread_create(&tid2, NULL, tree_generation, (void *)child2_data);

    // Main thread waits for the two initial branches to finish
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // Clean up root
    free(root_data->history);
    free(root_data);

    return 0;
}