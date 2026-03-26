#include <iostream>

// tell compiler to automatically look inside std 
// this way we avoid std::cout and write directly cout
using namespace std;

int main(){

    cout << "Welcome to my cpp repo" << endl;

    // lets prompt the user
    cout << "Please enter your age and random number: " <<  flush; // flush to the screen immediately 

    int age;
    float random;

    cin >> age >> random;

    cout << "You are " << age << "years old and you entered number as : " << random << endl;

    cerr << "just a random error mesage " << endl;

    return 0;
}