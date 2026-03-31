/*
    Full autonomy lambda functions looks like
    [capture_list] (parameter_list) -> return_type { function_body }
*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main(){
    vector<int> numbers = {1, 2, 3, 4, 5};
    int multiplier = 10;
    for_each(numbers.begin(), numbers.end(), [multiplier](int n){
        cout << n * multiplier << " "; // Prints 10 20 30 40 50
    });
    cout << endl;

    int evenCount = 0;
    int oddCount = 0;
    // in order to modify these values we should pass them by reference
    for_each(numbers.begin(), numbers.end(), [&evenCount, &oddCount](int n) {
        if (n % 2 == 0) {
            evenCount++; // Modifies the original 
        } else {
            oddCount++;  // Modifies the original 
        }
    });
    cout << "Found " << evenCount << " even numbers and " << oddCount << " odd numbers." << endl;
    return 0;
}