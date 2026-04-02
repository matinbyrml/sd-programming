#include <iostream>
#include <memory>
#include <string>

using namespace std;

struct Player {
    string name;
    Player(string n) : name(n) { cout << "[+] Player " << name << " spawned!" << endl; }
    ~Player() { cout << "[-] Player " << name << " deleted from memory!" << endl; }
};

int main(){
    shared_ptr<Player> ownerPtr =  make_shared<Player>("Mario");
    weak_ptr<Player> observerPtr(ownerPtr);

    cout << "String Count:" << ownerPtr.use_count() << endl;
    /*
    .lock(): This is the magic key. If the memory is still alive, 
    .lock() temporarily converts your weak_ptr into a full shared_ptr so you can safely read the data. 
    If the memory is dead, .lock() just returns a safe nullptr.*/
    if(shared_ptr<Player> tempPtr =  observerPtr.lock()){
        cout << "Observer sees the player is alive! Name: " << tempPtr->name << endl;
    }
    /*
    .reset(): This explicitly clears the pointer, setting it to nullptr. 
    (Note: .reset() also works on shared_ptr and unique_ptr to force them to drop their memory early!)
    */

    ownerPtr.reset();

    // destruction of mario triggers here
    if (observerPtr.expired()) { //
        cout << "Observer reports: The player is gone (expired is TRUE)." << endl;
    }
    if (shared_ptr<Player> tempPtr2 = observerPtr.lock()) { //
        cout << "This line will never print." << endl;
    } else {
        cout << "Lock failed securely. No dangling pointer crash!" << endl;
    }

    return 0;
}