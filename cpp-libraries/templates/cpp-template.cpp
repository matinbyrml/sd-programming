#include <iostream>
#include <string>

using namespace std;

template <typename T>
void printSwap( T& a, T& b){
    T temp = a;
    a = b;
    b = temp;
    cout << "Swapped! Now A is: " << a << " and B is: " << b << endl;
}

template <typename T, typename U> // we can use multiple generic types
class PairBox{
    private: 
        T firstItem;
        U secondItem;
    public:
        PairBox( T first, U second): firstItem(first), secondItem(second){};
        void display() {
            cout << "Box contains: [" << firstItem << "] and [" << secondItem << "]" << endl;
        }    
};
int main() {
    int x = 10, y = 50;
    printSwap(x, y); // The compiler secretly generates printSwap(int, int)

    string s1 = "Batman", s2 = "Joker";
    printSwap(s1, s2); // The compiler secretly generates printSwap(string, string)


    // We MUST specify the types in <> for classes! [cite: 3319]
    PairBox<int, string> myBox(99, "Red Balloons");
    myBox.display();

    PairBox<double, char> smallBox(3.14, 'Z');
    smallBox.display();

    return 0;
}