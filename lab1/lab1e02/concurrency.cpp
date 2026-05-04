#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>




int main(){
  //A PID is a unique identification number assigned by the Operating System to every single active process.
  pid_t pid;
  int i;
  setbuf (stdout,0); // print immediately, no buffer
  for (i=1; i<=3; i++){
    switch (i) {
      // fork() creates an exact replica. The child starts executing from the exact same line 
      // the fork happened. It does not start from the top of main()
      case 1: fork(); break;
      // fork() returns 0 to the child, and a number greater than 0 to the parent.
      // This is how we write if statements to make them do different things
      case 2: pid=fork(); if (pid!=0) system ("echo case 2"); break; // system() pauses the parent. 
      //It delegates a command to the terminal and waits for it to finish
      // exec() is a point of no return. Once a process calls execlp, it is destroyed and replaced by the new program.
      // It will never return to the loop
      case 3: execlp ("echo", "myPgrm", "case 3", NULL); break;
    }
  }
  return (0);
} 

/*
(i=1)       P0
           /  \
(i=2)    P0    P1
        / \    / \
(i=3)  P0 P2  P1 P3
*/


