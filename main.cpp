#include <iostream>
#include <thread>
#include <fstream>
#include <memory>
#include <vector>
#include <mutex>

// global mutex lock for synchronisation
std::mutex global_vector_lock;

// message queue corresponding to each process
std::vector<std::vector<int>> msg_queue;


// BSS process handler
void bss_process(int id)
{
    global_vector_lock.lock();

        // critical section
        std::cout << "critical section : " << id << std::endl;
        if(msg_queue[0].empty()) {
            msg_queue[0].push_back(69);
        } else {
            std::cout << msg_queue[0].back() << std::endl;
            msg_queue[0].pop_back();
        }
    
    global_vector_lock.unlock();

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

    // making shared global vector
    msg_queue.resize(n);

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