#include <iostream>
#include <string>

using namespace std;

struct Sensor {
    string name;

    Sensor(string name) {
        this->name = name;
        cout << "Sensor " << name << " created CONSTRUCTED on heap." << endl;
    }

    ~Sensor() {
        cout << "Sensor " << name << " destroyed DESTRUCTED on heap." << endl;
    }

};

int main(){
    // Dynamically allocate a Sensor object on the heap
    Sensor *sensor1 = new Sensor("Temperature");
    // Dellocate the Sensor object to free up memory
    delete sensor1;

    sensor1 = nullptr; // Set pointer to nullptr after deletion to avoid dangling pointer
    /*
        Dangling pointer :
        When you called delete s1;, it destroyed the object on the Heap... but it did not destroy the pointer s1 on the Stack!
    */

    // How to allocate an array?

    int* dataArray = new int[5](); // Dynamically allocate an array of 5 integers on the heap
    dataArray[0] = 10;
    dataArray[1] = 20;

    // we use delete[] to deallocate memory allocated for an array
    delete[] dataArray; // Deallocate the array memory

    Sensor* s2 = new Sensor("Pressure_01");
    Sensor* s3 = s2; // s3 is now pointing to the EXACT same memory as s2
    
    delete s2; // We delete the object
    s2 = nullptr; // We safely nullify s2 
    
    // THE TRAP: s3 is STILL pointing to the deleted memory! It is a Dangling Pointer!
    if (s3 != nullptr) {
        cout << "DANGER: s3 thinks it is valid, but points to dead memory!" << endl;
        // cout << s3->name; // If we uncomment this, the program will likely crash
    }
}