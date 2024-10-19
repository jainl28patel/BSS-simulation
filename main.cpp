#include <iostream>
#include <thread>
#include <fstream>
#include <memory>
#include <vector>


void bss_process(int id)
{
    std::cout << id << std::endl;
    return;
}

int main()
{
    // taking required files
    std::string input_file = "input.txt";
    std::string output_file = "./results/output.txt";

    // set input output from file
    std::ifstream in_file(input_file);
    std::ofstream out_file(output_file);

    // number of processes
    int n = 4;

    // spawning threads
    std::vector<std::unique_ptr<std::thread>> processes;

    for(int i = 0; i < n; i++)
    {
        processes.emplace_back(std::make_unique<std::thread>(bss_process,i));
    }

    for(auto &process_thread : processes)
    {
        process_thread.get()->join();
    }

    std::cout << "All thread done" << std::endl;

    // closing files
    in_file.close();
    out_file.close();

    return 0;
}