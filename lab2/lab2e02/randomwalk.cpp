#include <iostream>
#include <fstream>

using namespace std;

struct ExamRecords {
    int id;
    long registerNumber;
    char surname[31];
    char name[31];
    int examMark;
};

int main (int argc, char* argv[]){
    if (argc != 2){
        cout << "Usage: " << argv[0] << " and binary file name as file.bin " << endl;
        return 1;
    }

    // open the file in input, output and binary mode
    fstream myFile(argv[1], ios::in | ios::out | ios::binary);

    if(!myFile.is_open()){
        cout << "Error opening the file" << endl;
        return 1;
    }

    char command;
    int n;
    ExamRecords record;

    cout << "Type the commands"  << endl;
    
    while (true){
        cout << "> ";
        cin >> command;

        if(command == 'E' || command == 'e'){
            break;
            //stop the program execution;
        }

        cin >> n; // which student id?

        long offset = (n-1) * sizeof(ExamRecords);

        myFile.clear();

        switch (command)
        {
        case 'R':
        case 'r':
            myFile.seekg(offset, ios::beg);

            if(myFile.read((char*)&record, sizeof(ExamRecords))){
                cout << record.id << " "
                         << record.registerNumber << " "
                         << record.surname << " "
                         << record.name << " "
                         << record.examMark << endl;
                } else {
                    cout << "Error: Record " << n << " not found!" << endl;
                }
            break;
        case 'W':
        case 'w':
            cout << "Enter the data for the exam record" << endl;
            cin >> record.id >> record.registerNumber >> record.surname >> record.name >> record.examMark;

            myFile.seekp(offset, ios::beg);

            myFile.write((char*)&record, sizeof(ExamRecords));
            break;
        
        default:
            cout << "Unknown command" << endl;
            break;
        }

    }

    myFile.close();
    return 0;
}