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
}