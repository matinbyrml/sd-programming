
#include <functional>
#include <iostream>
#include <vector>


using namespace std;
using std::cout;
using std::endl;


auto l1 (int& x) {
  return [&] { std::cout << x << '\n'; };
}


int main() {
  {
    std::string str = "opeNai";


    std::sort(str.begin(), str.end(), [](char a, char b) {
        return std::tolower(a) < std::tolower(b);
    });


    std::cout << "L1: " << str << std::endl;
  }


  {
    int arr[] = {5, 2, 8, 1, 9, 3};


    int size = sizeof(arr) / sizeof(arr[0]);


    std::sort(arr, arr + size, [](int a, int b) {
        return a > b;
    });


    std::cout << "L2: ";
    for (int i = 0; i < size; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
  }


  {
    int i = 3;
    auto f = l1(i);
    i = 5;
    cout << "L3: ";
    f();
  }


  {
    std::vector<int> c{1, 2, 3, 4, 5, 6, 7};
    int x = 5;
    c.erase(std::remove_if(c.begin(), c.end(), [x](int n) { return n < x; }), c.end());
 
    std::cout << "L4: ";
    std::for_each(c.begin(), c.end(), [](int i) { std::cout << i << ' '; });
    std::cout << '\n';
  }


  {
    // Lambda with recursion
    const int val = 10;
    
    auto l_fib = [](int n)
    {
        std::function<int(int, int, int)> fib = [&](int n, int a, int b)
        {
            return n ? fib(n - 1, a + b, a) : b;
        };
        return fib(n, 0, 1);
    };


    cout << "L5: ";
    for (int i{1}; i <= val; ++i)
        std::cout << l_fib(i) << (i < val ? ", " : "\n");
  }
  
  return 0;
}


