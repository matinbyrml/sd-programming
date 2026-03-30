#include <iostream>
#include <string>
#include <cctype>

using namespace std;

int main(){
    // this is  C like, old schoold pointer for strings as chars
    char c_string[] = "batman";
    
    char *ptr =  c_string; //points to the first letter p

    while(*ptr != '\0'){
        *ptr = toupper(*ptr);
        ptr++;
    }

    cout << "Result: " << c_string << endl;

    // modern version for c++
    string cpp_string = "superman";
    for(auto it = cpp_string.begin(); it != cpp_string.end(); ++it){
        *it = toupper(*it);
    }

    cout << "Result: " << cpp_string << endl;

}