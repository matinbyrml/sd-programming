#include <iostream>

using namespace std;

void printValue(int *ptr) {
    cout << "Value is: " << *ptr << endl;
}

int main(){
    const int myAge =  25;
    // we cannot pass myAge to printValue function
    // sice *ptr is non const

    // it casts away the const, converts constant into non const type
    int *age_ptr =  const_cast<int*>(&myAge);
    printValue(age_ptr);

    // for Reinterpret cast check lab2 for better explanation
}