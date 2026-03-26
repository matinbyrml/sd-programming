#include <iostream>

using namespace std;

int main() {
   int i = 10;
   int *ptr = &i; 
    *ptr = 20; // i is 20 now

    int &r = i; // a reference, a nickname for i, r and i are the same memory adress
    r = 10; // now also i is 10
    int myScores[] = {10, 20, 30};

    cout << "Loop by Value:" << endl;
    // 'score' is just a temporary copy
    for (auto score : myScores) {
        score = score + 5; // Adds 5 to the copy, array is safe
    }
    
    // Let's print to see if the array changed
    cout << "Array after value loop: ";
    for (auto score : myScores) cout << score << " "; 
    cout << endl; // Prints: 10 20 30


    cout << "\n Loop by Reference" << endl;
    // 'score' is a direct nickname to the array memory
    for (auto &score : myScores) {
        score = score + 5; // Directly alters the array!
    }

    // Let's print again
    cout << "Array after reference loop: ";
    for (auto score : myScores) cout << score << " "; 
    cout << endl; // Prints: 15 25 35

    return 0;
}