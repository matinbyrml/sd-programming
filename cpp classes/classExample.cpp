/*
    In C++, struct and class are practically identical, with one massive difference: 
    If you don't type an access specifier, a struct makes everything public by default, 
    while a class makes everything private by default.
*/

#include <iostream>
#include <cstdlib>

using namespace std;

class Sensor{
    private:
        int id;
        int* dataHistory; //dynamic array
    public: 
        Sensor(int id, int size){ // constructor, like in java
            this->id = id;
            dataHistory = (int*)malloc(size * sizeof(int)); //use malloc to allocate memory
            cout << "Sensor is constructed" << endl;
        }
        
        int getId(){
            return this->id;
        }
        
        // Deconsturctor: Why use it? If your constructor uses malloc or new to grab dynamic memory, 
        // your destructor MUST contain the free or delete command to prevent memory leaks.
        ~Sensor() {
            free(dataHistory);
            cout << "Sensor is destroyed from the memory" << endl;
        }
};

struct Rectangle {
    private:
        int w;
        int h;
    // Publiv by default
    public:
        Rectangle(int w, int h){
            this->w=w;
            this->h=h;
        }
        // Method decleration
        int getArea();
};
// Method definition
int Rectangle::getArea(){
    return this->w*this->h;
}

int main(){
    // Lets create an instance inside a scope:
    {
     Sensor s1(1, 10);
    }
    // Now object is out of scope; deconstructor is triggered
    // In C++, objects are destroyed in the exact reverse order of their construction
    // stack is LIFO


    Rectangle rect(10, 5); // Calls the class constructor
    
    // Calls the externally defined method
    cout << "Rectangle Area: " << rect.getArea() << endl;
    return 0;
}