#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main () {
  int i, j=-1;


  setbuf (stdout,0);
  for (i=0; i<5; i++) {
    printf ("i=%d\n", i);
    // Child goes inside the if block.
    if (!fork()) {
      for (j=2; j>0; j--) {
        fork();
        // execlp treats the second argument ("i") as the program name, and only prints the arguments after it. 
        // So this command simply prints the letter "j" to the screen
        execlp ("echo", "i", "j", (char *) 0); 
      }
    } else {
      printf ("j=%d \n", j);
    }
  }


  return (1);
}
