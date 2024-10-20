#include <iostream>
#include <thread>
#include <fstream>
#include <memory>
#include <vector>
#include <mutex>
#include "main.h"

int n; //no of processes

// global mutex lock for synchronisation
std::mutex global_vector_lock;

// message queue corresponding to each process
std::vector<std::vector<int>> msg_queue;

// list of processes
std::vector<process> p_list;

std::vector<message> msg_list;

message::message(){
    msg_id=-1;
    sender_id=-1;
}

bool check_timestamp(std::vector<int> &timestamp,std::vector<int> &p_clock,int sender_id){
    int n=timestamp.size();
    for(int i=0;i<n;i++){
        if(i==sender_id)continue;
        if(p_clock[i]<timestamp[i])return false;
    }
    return p_clock[sender_id]==timestamp[sender_id]-1;
}

void process::recieve_message(event &e){
    bool is_msg_found=0;
    while(true){
        global_vector_lock.lock();
        for(int cnt=0;cnt<n;cnt++){
            for(int i=0;i<msg_queue[pid].size();i++){
                if(check_timestamp(msg_list[msg_queue[pid][i]].timestamp,p_clock,e.sender_pid)){
                    msg_queue[pid].erase(msg_queue[pid].begin()+i);
                    if(msg_queue[pid][i]==stoi(e.msg_id))is_msg_found=1;
                }
            }
        }
        global_vector_lock.unlock();
        if(is_msg_found)break;
    }
}

void process::send_message(event &e){
    message msg;
    msg.msg_id=stoi(e.msg_id); // set msg id
    msg.sender_id=pid;
    p_clock[pid]++;
    msg.timestamp=p_clock;
    msg_list.push_back(msg);
    global_vector_lock.lock();
    for(int id=0;id<n;id++){
        if(id==pid)continue;
        out_file<<
        msg_queue[id].push_back(msg.msg_id);
    }
    global_vector_lock.unlock();
}

void process::run(std::vector<event> &instructions){
    for(auto &inst:instructions){
        if(inst.is_send_event){ //send 
            send_message(inst);
        }
        else{ //receive
            recieve_message(inst);
        }
    }
}


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