#include <iostream>
#include <string>

// We ONLY include the header. 
// The header will automatically pull in the .hpp file for us
#include "Box.h" 

using namespace std;

int main() {
    // 1. Using an Integer Box
    Box<int> intBox(42);
    cout << "Integer Box contains: " << intBox.getItem() << endl;

    // 2. Using a String Box
    Box<string> stringBox("Hello Templates!");
    cout << "String Box contains: " << stringBox.getItem() << endl;

    return 0;
}