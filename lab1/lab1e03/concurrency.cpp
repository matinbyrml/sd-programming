#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main () {
  int i;
  setbuf (stdout,0);
  for (i=0; i<2; i++) {
    printf ("i=%d\n", i);
    // Any number that is not 0 is considered True, and 0 is considered False
    if (fork())             // Call # 1:  returns 0 to child and p_id to the parent, so parents and children will do different stuff, parent creates a child here!
      fork ();              // Call # 2:  creates another child here 
    else // child processes skips if statement and jumps to else statement 
      system ("echo i=i");  // Call # 3 The system() function just pauses the process, runs the terminal command, and then returns to the code. This means any process that runs system("echo i=i") will eventually continue to the next iteration of the loop
  }
  return (1);
}
 /*
 (i=0)               P0 
                  /    \
               P1(sys)  P2
              /         |  \
(i=1)       /           |    \
         P0             P1     P2
        /  \           /  \   /  \
     P3(sys) P4     P5(sys) P6 P7(sys) P8

     "sys" means that process executed the system() command
 */