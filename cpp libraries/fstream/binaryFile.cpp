#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

struct Student{
    char firstName[20];
    char lastName[20];
    int studentId;
};

int main(){
    string filename = "students.bin";

    cout << "Writing into a binary file";
    // here is the trick, we have to use several stuff now
    ofstream outFile(filename, ios::out  | ios::binary | ios::app);

    if(!outFile.is_open()){
        cerr << "Some problem with opening file to write" << endl;
        return 1;
    }

    // lets create some instances 
    Student s1, s2;
    strcpy(s1.firstName, "Matin");
    strcpy(s1.lastName, "Bayramli");
    s1.studentId = 321321;

    strcpy(s2.firstName, "Bayramli");
    strcpy(s2.lastName, "Matin");
    s2.studentId = 123123;

    outFile.write(reinterpret_cast<char*>(&s1), sizeof(Student));
    outFile.write(reinterpret_cast<char*>(&s2), sizeof(Student));

    outFile.close();

    cout<< "Lets read the file now" << endl;

    ifstream inFile(filename, ios::in | ios::binary);
    if(!inFile.is_open()){
        cerr << "Error opening the binary file to read";
        return 1;
    }

    Student buffer; // temporary class to read lines

    while(inFile.read((char*)&buffer, sizeof(Student))){
        cout << "Read student: " << buffer.firstName << " " << buffer.lastName << " "
        << buffer.studentId << endl;
    }
    inFile.close();

    return 0;


}