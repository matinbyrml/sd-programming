#include <iostream>

using namespace std;

int main() {
    unsigned int u = 19;
    int i = -42;
    
    cout << "--- The Math Trap (Fixed!) ---" << endl;
    
    cout << "Unsafe (u + i): " << u + i << endl; 
    
    cout << "C-Style cast: " << (int)u + i << endl;
    
    cout << "C++ static_cast: " << static_cast<int>(u) + i << endl;

    return 0;
}