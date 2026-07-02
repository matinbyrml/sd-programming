/*
    A C++ program executes four threads: TA, TB, TC, and TD. These threads are cyclical, run forever, and
    cooperate to generate sets of symbols on subsequent lines of the standard output. Each one of them can
    display one single character (an ‘A’, ‘B’, ‘C’, or ‘D’ respectively, and eventually a new line) for each
    iteration of their main cycle. Each line must have the following format: {A|B}+{C|D}
    This means that for each sequence, there are:
    
    {A|B}+: One or more symbols A or B (i.e., at least A or B and then maybe others A or B).
    
    {C|D}: One symbol between C or D.
    
    Each sequence is terminated by a “new line” character.
    The following is a correct example of the execution of such a program:
    ABC
    AC
    BD
    AABBBBC
    ABABABAD
*/

#include <iostream>
#include <thread>
#include <semaphore>
#include <chrono>

using namespace std;

// --- Synchronization Primitives ---
// sab controls access to printing and locks out C/D when needed
counting_semaphore<1> sab(1); 

// scd signals that the first A or B has printed, opening the gate for C/D
counting_semaphore<1> scd(0); 

// --- Shared State ---
int count_ab = 0; // Tracks how many A's or B's have printed in the current line


void TA() {
    while (true) {
        sab.acquire();      // Lock the printing phase
        
        cout << "A";
        count_ab++;
        
        if (count_ab == 1) {
            scd.release();  // If this is the FIRST char, enable C/D to get ready
        }
        
        sab.release();      // Unlock, allowing another A, B, or C/D to proceed
        
        // Minor delay to force random interleaving on the console
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void TB() {
    while (true) {
        sab.acquire();      // Lock the printing phase
        
        cout << "B";
        count_ab++;
        
        if (count_ab == 1) {
            scd.release();  // If this is the FIRST char, enable C/D to get ready
        }
        
        sab.release();      // Unlock, allowing another A, B, or C/D to proceed
        
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void TC() {
    while (true) {
        scd.acquire();      // Wait until at least one A or B has printed
        
        sab.acquire();      // CRITICAL: Lock out A and B so they can't print anymore
        
        cout << "C\n";      // Print C and end the line
        count_ab = 0;       // Reset the state for the next line
        
        sab.release();      // Unlock A and B to start a new line
    }
}

void TD() {
    while (true) {
        scd.acquire();      // Wait until at least one A or B has printed
        
        sab.acquire();      // CRITICAL: Lock out A and B so they can't print anymore
        
        cout << "D\n";      // Print D and end the line
        count_ab = 0;       // Reset the state for the next line
        
        sab.release();      // Unlock A and B to start a new line
    }
}

int main() {
    cout << "Starting Sequence Generator...\n\n";

    // Launch threads
    thread tA(TA), tB(TB), tC(TC), tD(TD);

    // Let them run indefinitely
    tA.join();
    tB.join();
    tC.join();
    tD.join();

    return 0;
}