#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(){
    string filename = "students.txt";

    cout << "Lets write something on the file" << endl;

    ofstream outFile(filename, ios::out);
    if(!outFile.is_open()){
        cerr << "Cant open file to write" << endl;
        return 1;
    }

    // we can write to the file like we write to cout
    outFile << "Matin Bayramli 321321";
    outFile << "Bayramli Matin 123123";

    outFile.close();

    cout << "Reading from a file now";

    ifstream inFile(filename);

    if(!inFile.is_open()){
        cerr << "Problem during opening the file to read" << endl;
        return 1;
    }

    string firstName, lastname;
    int studentid;

    while(inFile >> firstName >> lastname >> studentid){
        cout << "Reading student" << firstName << " " << lastname << " with id: " << studentid << endl;
    }

    inFile.close();

    return 0;
}