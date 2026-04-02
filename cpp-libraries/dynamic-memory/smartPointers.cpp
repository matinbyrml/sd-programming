#include <iostream>
#include <string>
#include <memory>

using namespace std;

struct Sensor {
    string name;
    Sensor(string n){
        name = n;
        cout << name << "Created" << endl;
    }
    ~Sensor() { cout << "[-] " << name << " Destroyed automatically!" << endl; }
};

int main(){
    // Unique pointers
    /*
    This pointer strictly owns the object it points to. 
    Only one unique_ptr can point to the memory at a time. 
    When it goes out of scope, the memory is instantly freed.
    */
   {
    unique_ptr<Sensor> uPtr(new Sensor("Temperature"));
    cout << "Reading from unique pointer" << uPtr ->name << endl;
   }

   // Shared pointers 
   /*(Communal Ownership): What if multiple parts of your code need to look at the same object?
   A shared_ptr keeps a secret Reference Count. Every time you copy the pointer, the count goes up. 
   Every time a pointer is destroyed, the count goes down. 
   When the count finally hits 0, the memory is safely deleted.*/
   shared_ptr<Sensor> sPtr1 =  make_shared<Sensor>("Pressure");
   cout << "Count after creating sPtr1: " << sPtr1.use_count() << endl; // Prints 1
   {
    shared_ptr<Sensor> sPtr2 = sPtr1; //
        cout << "Count after copying to sPtr2: " << sPtr1.use_count() << endl; // Prints 2
        
    } // sPtr2 goes out of scope and is destroyed. The counter goes down to 1.
    cout << "Count after sPtr2 died: " << sPtr1.use_count() << endl; // Prints 1
    
}