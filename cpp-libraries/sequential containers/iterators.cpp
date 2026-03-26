#include <iostream>
#include <vector>
#include <list>
#include <string>

using namespace std;

int main(){
    // Lets create a vector
    vector<string> words = {"Hello", "C++", "world!"};

    for(auto it = words.begin(); it != words.end(); it++){
        cout << *it << " ";
    }

    // now we reverse it 

    for( auto it = words.rbegin(); it != words.rend(); it ++){
        cout << *it << " ";
    }

    // with lists we can navigate with ++ operator, no math tho since no random access

    list<int> numbers =  {10,20,30,40};
    auto listIt =  numbers.begin();

    listIt++;
    listIt++;

    cout << "The 3rd item in the list is: " << *listIt << endl; // Prints 30

    // we have to be careful with iterators, since we can add or remove elements
    // out iterator might be invalidated

    vector<int> data = {1,2,3,4,5,6};

    auto it = data.begin();

    while(it != data.end()){
        if(*it %2 == 0){
            // now we match the condition we should carefully erase it
            // erase() returns a new correct iterator
            it = data.erase(it);
        }
        else{
            it ++;
        }
    }

    cout << "We have deleted the even numbers, now: ";
    for(auto val: data) cout << val << " ";
    cout << endl;

    return 0;
}

