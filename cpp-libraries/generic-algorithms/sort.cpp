#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

struct Student {
    string name;
    int grade;
};

int main(){
    vector<Student> classRoom  = {
        {"Bruce", 18},
        {"Antonio", 30},
        {"Matin", 0}
    };

    sort(classRoom.begin(), classRoom.end(), [](const Student&a, const Student& b){
        return a.grade > b.grade;
    });

    for (const auto& s : classRoom) {
        cout << s.name << " - " << s.grade << endl;
    }
}