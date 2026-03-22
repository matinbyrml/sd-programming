#include <iostream>
#include <cstdlib>



/*
The active selection `using namespace std;` is a directive in C++ that allows the programmer to use all the names (such as functions, classes, and objects) defined in the `std` namespace without explicitly qualifying them with the `std::` prefix.

In C++, the Standard Library is organized under the `std` namespace to avoid naming conflicts with user-defined identifiers. By including `using namespace std;`, you can directly use standard library components like `cout`, `cin`, `string`, and others without needing to prepend `std::` to each of them. For example, instead of writing `std::cout << "Hello, World!";`, you can simply write `cout << "Hello, World!";`.

While this directive can make the code more concise, it is generally recommended to use it cautiously, especially in larger projects. This is because it imports all the names from the `std` namespace into the global namespace, which can lead to potential naming conflicts or ambiguities if there are similarly named identifiers in the code. A more selective approach, such as `using std::cout;`, is often preferred to limit the scope of imported names.*/
using namespace std;


void sortArray(float* vet, int n);

int main(int argc, char* argv[]){
    if(argc != 2){
        cout  << "Error usage " << argv[0] << "num of elements" << endl;
        return 1;
    }
    // convert to an integer, command line args are strings
    int n = atoi (argv[1]);


    float* vet;

    vet = (float *) malloc ( n * sizeof(float));

    if(vet == nullptr){
        cout << "Memory allocation error" << endl;
        exit(1);
    }

    cout << "Please enter " << n  << "real numbers" << endl;
    for (int i = 0; i<n; i++){
        cin >> vet[i];
    }

    sortArray(vet, n);
     
    for(int i = 0; i<n; i++){
        cout << vet[i] << " ";
    }
    cout << endl;

    free(vet);
    return 0;
}


void sortArray (float* vet, int n){
    for (int i = 0; i<n-1; i++){
        for(int j= 0; j< n-i-1; j++){
            if (vet[j] > vet[j + 1]) {
                // Swap the values if they are in the wrong order
                float temp = vet[j];
                vet[j] = vet[j + 1];
                vet[j + 1] = temp;
            }
        }
    }
}