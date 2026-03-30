#include <iostream>
#include <map>
#include <string>


using namespace std;

int main(){
    map<string,int> wordCount;

    string document[] = {"apple", "is", "apple", "and", "banana", "apple", "the"};

    for(string word: document){
        wordCount[word]++;
    }

    //lets see the map

    for(const auto &item: wordCount){
        cout << item.first << " appears " << item.second << " times." << endl;
    }

    // lets search the map

    auto searchIt = wordCount.find("zebra");

    if (searchIt != wordCount.end()) {
        cout << "Found it! Zebra count: " << searchIt->second << endl; //
    } else {
        cout << "'zebra' was not found in the document." << endl;
    }

    return 0;
}