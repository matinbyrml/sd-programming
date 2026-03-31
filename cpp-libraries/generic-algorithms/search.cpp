/*
    Almost every algorithm follows the structure of 
    std::algorithm_name( c.begin(), c.end(), target_or_rule );
*/

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main(){
    vector<int> scores = {12, 11, 13, 443, 33};

    auto it = find(scores.begin(), scores.end(), 443);
    if(it!= scores.end()){
        cout << "Found the exact score" << endl;
    }
    // lambda function as a predicate
    auto it2 = find_if(scores.begin(), scores.end(), [] (int s){
        return s<50;
    } );

    if(it2 != scores.end()){
        cout << "Found with a predicate" << *it << endl;
    }

    // return a bool if it works for all the elements
    bool all_passed = all_of(scores.begin(), scores.end(), [](int s) {
        return s >= 40;
    });

    if (all_passed) {
        cout << "Good news: Everyone passed the baseline 40 points!" << endl;
    }

    return 0;
}