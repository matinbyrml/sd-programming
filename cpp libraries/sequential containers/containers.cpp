#include <iostream>
#include <vector>
#include <list>
#include <string>

using namespace std;

int main(){
    // We look at vector now
    vector<string> vault; // vector size is flexible like a dynamic array
    // It creates a temporary, invisible std::string object, grabs dynamic memory on the Heap, and copies 'G', 'o', 'l', 'd' into it.
    vault.push_back("Gold"); // 
    vault.emplace_back("Diamonds"); // build diamond directly inside

    // vectors do not suport push front
    vault.insert(vault.begin(), "Silver"); // We force it into the front using insert
    
    cout << "Vector size is: " << vault.size() << endl;
    cout << "First item: " << vault.front() << endl; // Silver
    cout << "Last item: " << vault.back() << endl;   // Diamonds
    cout << "Middle item: " << vault.at(1) << endl;  // Gold

    // to erase an element we use 
    vault.pop_back(); // deletes diamont
    cout << "After pop back: Vector size is: " << vault.size() << endl;
    cout << "First item: " << vault.front() << endl; // Silver
    cout << "Last item: " << vault.back() << endl;   // Diamonds
    cout << "Middle item: " << vault.at(1) << endl;  // Gold
    
    vault.erase(vault.begin());
    cout << "After erase Vector size is: " << vault.size() << endl;
    cout << "First item: " << vault.front() << endl; // Gold
    cout << "Last item: " << vault.back() << endl;   // Gold
    // cout << "Middle item: " << vault.at(1) << endl;  // error 

    for (auto i: vault){
        cout << i << " ";
    }
    cout << endl;

    vault.clear(); // Nukes everything
}