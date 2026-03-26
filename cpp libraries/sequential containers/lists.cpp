#include <iostream>
#include <list>
#include <string>

using namespace std;

int main(){
    // Now we take a look at c++ lists, they are double linked lists
    // Since they are llists, random access is not very efficient

    list<string> train;

    train.push_back("Passanger Car");
    train.push_front("Engine");
    train.push_back("Carboose");

    //if i wanna insert in the middle  i can  create a pointer pointing to the head

    auto it = train.begin(); // points to the engine now
    it++; // we skip 1, so points to the passenger car now

    train.insert(it, "Dining vagon");

    // lets check the list now
    for(auto vagon: train){
        cout << "[" << vagon << "] -> ";
    }
    cout << "END" << endl;

    train.pop_front(); // Removes the engine
    train.pop_back();  // Removes the carboose 

    // this is the universal way to loop in c++
    for (auto iter = train.begin(); iter != train.end(); ++iter) {
        cout << "[" << *iter << "] -> "; // Dereference (*) to read the data
    }
    cout << "END" << endl;
    return 0;
}
