#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

struct ExamRecords {
    int id;
    long registerNumber;
    char surname[31];
    char name[31];
    int examMark;
};

int main(int argc, char* argv[]) {

    if (argc != 4) {
        cout << "Usage: " << argv[0] << "names of 3 files" << endl;
        return 1;
    }

    ExamRecords record;


    
    ifstream inText(argv[1]);
    if (!inText.is_open()) {
        cout << "Error opening " << argv[1] << endl;
        return 1;
    }

    ofstream outBinary(argv[2], ios::binary);
    if (!outBinary.is_open()) {
        cout << "Error opening " << argv[2] << endl;
        return 1;
    }

    // The text read loop: Stops automatically when it hits the end of the file
    while (inText >> record.id >> record.registerNumber >> record.surname >> record.name >> record.examMark) {
        // Write the raw bytes of the struct directly to the binary file
        // We cast the memory address of 'record' to a char pointer, and tell it how many bytes to write.
        outBinary.write((char*)&record, sizeof(ExamRecords));
        /*
            a char is simply a block of memory that is exactly 1 byte wide.
            Because write() only deals in raw bytes, its creators designed it to strictly accept a 
            char * (a pointer to a character array). It basically says, "Give me a pointer to the first byte, 
            tell me how many bytes to walk forward, and I will copy them."

            // The modern C++ way:
            myFile.read(reinterpret_cast<char*>(&record), sizeof(ExamRecords));
        */
    }

    // Always close files when done!
    inText.close();
    outBinary.close();




    // Open file_2 for reading in BINARY mode
    ifstream inBinary(argv[2], ios::binary);
    if (!inBinary.is_open()) {
        cout << "Error opening " << argv[2] << endl;
        return 1;
    }

    // Open file_3 for writing (standard text mode)
    ofstream outText(argv[3]);
    if (!outText.is_open()) {
        cout << "Error opening " << argv[3] << endl;
        return 1;
    }

    // The binary read loop: .read() returns true as long as it successfully grabs a chunk of bytes
    while (inBinary.read((char*)&record, sizeof(ExamRecords))) {
        // Write the formatted text back out to file_3
        outText << record.id << " " 
                << record.registerNumber << " " 
                << record.surname << " " 
                << record.name << " " 
                << record.examMark << endl;
    }

    // Close the final files
    inBinary.close();
    outText.close();

    cout << "Conversion completed successfully!" << endl;

    return 0;
}