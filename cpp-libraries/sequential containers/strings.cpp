#include <iostream>
#include <string>
#include <cctype>

using namespace std;

int main(){
    string first = "Bruce";
    string last = "Wayne";

    string batman = first + " " + last;
    cout << "Hero " << batman << endl;

    for(auto &c: batman){
        if(isalpha(c)){
            c = toupper(c);
        }
    }

    cout << "Changed a bit: "  << batman << endl;
    string data = "Target located at sector 7G.";

    int pos =  data.find("sector");

    if(pos != string::npos){
        string location  =  data.substr(pos, 9);
        cout << "Extracted: " << location << endl; // Prints "sector 7G"
    }
    return 0;
}