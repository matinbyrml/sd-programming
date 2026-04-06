#include <iostream>

int main() {
    int seed = 10;
    
    // Captures 'seed' by REFERENCE
    auto generator1 = [&seed]() { 
        seed *= 2; 
        return seed; 
    };
    
    // Captures 'seed' by VALUE
    auto generator2 = [seed]() { 
        int seed2 = seed + 5; 
        return seed2; 
    };
    
    generator1(); 
    std::cout << generator1() << " "; 
    std::cout << generator2() << " "; 
    std::cout << seed; 
    
    return 0;
}