// copy constructor
/*
    Copy Constructor (C(const C&)): Creates a brand new object by duplicating an existing one.
*/
#include <cstddef>
class Buffer {
    private:
        int* data;
        size_t size;
    
    public:
        // PRIMARY CONSTRUCTOR
        Buffer(size_t s) {
            size = s;
            data = new int[size]; // Allocate Heap memory
        }
    
        // COPY CONSTRUCTOR
        Buffer(const Buffer& other) { //
            // 1. We must allocate brand new memory on the Heap for our copy
            this->size = other.size;
            this->data = new int[this->size]; 
            
            // 2. We must loop through all 1,000,000 integers and copy them! (Very slow: O(N))
            for (size_t i = 0; i < size; i++) {
                this->data[i] = other.data[i];
            }
        }
        // MOVE CONSTRUCTOR
        /*
            Move Constructor (C(C&&)): Creates a new object by "stealing" the memory/resources 
            from a temporary object that is about to die
        */
    Buffer(Buffer&& other) noexcept { //
        // 1. Steal the size and the Pointer! (Instantaneous: O(1))
        this->size = other.size;
        this->data = other.data; // We just copied the memory address, NOT the 1 million ints!
        
        // 2. SECURE THE HEIST (Crucial Step!)
        // We must disconnect the original object from the memory. 
        // If we don't, when 'other' dies, its destructor will run `delete[] data` and destroy our stolen array!
        other.size = 0;
        other.data = nullptr; 
    }
        // DESTRUCTOR
        ~Buffer() { delete[] data; }
    };