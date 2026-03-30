#include <vector>
#include <iostream>
#include <set>

using namespace std;

int main(){
    set<int> mySet;
    vector<int>  rawData = {9,2,5,2,9,1,5,5};

    for(int num: rawData){
        mySet.insert(num);
    }

    cout << "After insertion from vector to my set the size is :" << mySet.size() << endl;
    // since it is a normal set (not unordered), it is automatically ordered
    for( auto it = mySet.begin(); it != mySet.end(); ++it){
        cout << *it << " "; 
    }
    cout << endl;

    // we can search a specific entry in set
    if(mySet.count(5)){
        cout << "We have found the number 5" << endl;
    }
    // Using .find() to get an iterator
    auto searchIt = mySet.find(99);
    if (searchIt == mySet.end()) {
        cout << "The number 99 is NOT in the set!" << endl;
    } 
    // else we have it

    return 0;
}