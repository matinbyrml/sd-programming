 #include <iostream>

 using namespace std;

 void myswap(int &x, int &y){
    int temp = x;
    x = y;
    y = temp;
 }

 // Overloading example, consider like in java
 void print (int v){
    cout << "Printing an integer: " << v << endl;
 }

 void print(string v){
    cout << "Printing a string: " << v << endl;
 }

 // function with default arguments: like in javascript
 int sum( int x, int y, int z = 0, int w = 0){
    return x+y+z+w;
 }

 int main(){
    int a = 10, b = 20;

    myswap(a, b); // We don't need &a, &b here. The compiler handles it!
    cout << "After swap: a=" << a << ", b=" << b << endl;
    cout << "Sum of 3 numbers: " << sum(10, 15, 25) << endl;   // Uses w=0
 }