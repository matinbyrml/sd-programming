#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char* argv[]){
    if(argc!=3){
        cout << "Error on command line arguments" << endl;
        return 1;
    }

    int h = atoi(argv[1]); // tree height
    int n = atoi(argv[2]); // degree n
    pid_t pid;
    setbuf (stdout,0); // print immediately, no buffer
  
    for(int i = 0; i<h; i++){
        for( int j = 0; j<n; j ++){
            pid = fork();
            if (pid == 0) {
                // I AM THE CHILD.
                // I must break out of the 'j' loop so I don't spawn my own siblings.
                // Breaking sends me back to the outer 'i' loop, so I become a parent at the next level!
                break; 
            } else if (pid < 0) {
                printf("Fork failed!\n");
                exit(1);
            }
        }
        if (pid > 0) {
            // I AM THE PARENT.
            // I have finished my 'j' loop and successfully spawned 'n' children.
            // The instructions say: "The initial process generates n child jobs and exits."
            exit(0); 
        }
    }
    printf("I am a leaf process! My PID is: %d\n", getpid());
    return 0;
}