/*
    The file receives N file names on the command line
    Each file stores an undefined number of integers

    For every command-line argument, the main
    function must create a new thread

    Each thread opens its assigned file, iterates through
    the integers, updates its local sum and count, and
    prints the results to the standard output.

    The main thread waits until all worker threads
    have completed their tasks and aggregates the
    partial sums into a single global sum
*/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <numeric>

// hold file data and results
struct FileData {
    std::string filename;
    int count = 0;
    long long sum = 0;
};

void processFile(FileData& data){
    std::ifstream file(data.filename);
    if(!file.is_open()){
        std::cerr << "Error opening file" << std::endl;
        return;
    }

    int value;
    while(file >> value){
        data.sum += value;
        data.count++;
    }

    file.close();
    printf("File: %s | Count: %d | Sum: %lld\n",
        data.filename.c_str(), data.count, data.sum);
}

int main(int argc, char* argv[]){
    int n = argc -1;
    std::vector<FileData> results(n);
    std::vector<std::thread> threads;

    for(int i = 0; i<n;i++){
        results[i].filename =  argv[i+1];
        //threads start running here 
        threads.emplace_back(processFile, std::ref(results[i])); // pass by reference so results in main are updated
    }

    for(auto& t: threads){
        if(t.joinable()){
            t.join();
        }
    }

    long long totalCount = 0, totalSum = 0;

    for(const auto& res : results){
        totalCount += res.count;
        totalSum += res.sum;
    }
    std::cout << "Total Count: " << totalCount << std::endl;
    std::cout<< "Total Sum : " << totalSum<< std::endl;
    return 0;
}
