#include <iostream>
#include <thread>
#include <fstream>
#include <memory>
#include <vector>
#include <mutex>
#include <set>
#include <unistd.h>
#include <format>
#include <algorithm>
#include "utils.cpp"
#include "main.h"

int n; //no of processes

// global mutex lock for synchronisation
std::mutex global_vector_lock;
std::mutex process_log_lock;

// message queue corresponding to each process
std::vector<std::vector<message>> msg_queue;

// output log
std::vector<std::vector<std::string>> process_log;

// list of processes
std::vector<process> p_list;

// taking required files
std::string input_file = "input.txt";
std::string output_file = "./results/output.txt";

// set input output from file
std::ifstream in_file(input_file);
std::ofstream out_file(output_file);


message::message(){
    msg_id=-1;
    sender_id=-1;
}

bool check_timestamp(std::vector<int> &timestamp,std::vector<int> &p_clock,int sender_id){
    int m=timestamp.size();
    for(int i=0;i<m;i++){
        if(i==sender_id)continue;
        if(p_clock[i]<timestamp[i])return false;
    }
    return p_clock[sender_id]==timestamp[sender_id]-1;
}

void update_time(std::vector<int>& p_clock, std::vector<int>& timestamp)
{
    for(int i = 0; i < p_clock.size(); i++)
    {
        p_clock[i] = std::max(p_clock[i],timestamp[i]);
    }
}

std::string clock_to_string(std::vector<int> clock)
{
    std::string ret = "(";
    std::for_each(clock.begin()+1,clock.end(),[&](int time){
        ret += std::to_string(time);
    });
    ret += ")";
    return ret;
}

void process::recieve_message(event &e){
    if(this->recieved_msg.find(e.msg_id)!=this->recieved_msg.end()) return;
    bool is_msg_found=0;
    bool print_bss = false;
    bool print_app = false;
    message msg;

    while(true) {
        global_vector_lock.lock();
            for(auto i = msg_queue[pid].begin(); i!=msg_queue[pid].end(); i++)
            {
                message curr_msg = *i;
                if(curr_msg.msg_id==stoi(e.msg_id.substr(1))) {
                    is_msg_found = true;
                    msg = curr_msg;
                    print_bss = true;
                    msg_queue[pid].erase(i);
                    break;
                }
            }
        global_vector_lock.unlock();

            if(is_msg_found) break;

            // check local store
            for(auto i = this->pending_msg.begin(); i!=this->pending_msg.end(); i++)
            {
                message curr_msg = *i;
                if(curr_msg.msg_id==stoi(e.msg_id.substr(1))) {
                    is_msg_found = true;
                    msg = curr_msg;
                    print_bss = false;
                    this->pending_msg.erase(i);
                    break;
                }
            }

            if(is_msg_found) break;
        sleep(2);
    }

    if(check_timestamp(msg.timestamp, this->p_clock, msg.sender_id))
    {
        print_app = true;
    }
    else
    {
        print_app = false;
        this->pending_msg.push_back(msg);
    }

    if(print_bss) {
        std::string output = std::format("recv_B p{} m{} ", msg.sender_id, msg.msg_id) + clock_to_string(this->p_clock);
        process_log_lock.lock();
            process_log[pid].push_back(output);
        process_log_lock.unlock();
    }
    if(print_app) {
        update_time(this->p_clock, msg.timestamp);
        std::string output = std::format("recv_A p{} m{} ", msg.sender_id, msg.msg_id) + clock_to_string(this->p_clock);
        process_log_lock.lock();
            process_log[pid].push_back(output);
        process_log_lock.unlock();
    }

    // check for after effect
    bool koi_mila = true;
    while(koi_mila)
    {
        koi_mila = false;
        message temp_msg;
        for(auto i = this->pending_msg.begin(); i!=this->pending_msg.end(); i++)
        {
            message curr_msg = *i;
            if(check_timestamp(curr_msg.timestamp,this->p_clock,curr_msg.sender_id)) {
                temp_msg = curr_msg;
                koi_mila = true;
                this->pending_msg.erase(i);
                break;
            }
        }

        if(koi_mila)
        {
            update_time(this->p_clock, temp_msg.timestamp);

            std::string output = std::format("recv_A p{} m{} ", temp_msg.sender_id, temp_msg.msg_id) + clock_to_string(this->p_clock);
            process_log_lock.lock();
                process_log[pid].push_back(output);
            process_log_lock.unlock();
        }
    }

}

void process::send_message(event &e){
    message msg;
    msg.msg_id=stoi(e.msg_id.substr(1)); // set msg id
    msg.string_msg_id = e.msg_id;
    msg.sender_id=pid;
    p_clock[pid]++;
    msg.timestamp=p_clock;
    global_vector_lock.lock();

    std::string output = std::format("send m{} ", msg.msg_id) + clock_to_string(this->p_clock);
    process_log_lock.lock();
        process_log[pid].push_back(output);
    process_log_lock.unlock();

    for(int id=1;id<=n;id++){
        if(id==pid)continue;
        msg_queue[id].push_back(msg);
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
void bss_process(int id, std::vector<event> instructions)
{
    process this_process(id,n);
    for(auto inst : instructions) {
        if(!inst.is_send_event) {
            this_process.expected_msg.insert(inst.msg_id);
        }
    }
    this_process.run(instructions);
    return;
}

int main()
{
    // parse commands
    std::vector<std::vector<event>> process_events = parse_commands(in_file);


    // number of processes
    n = process_events.size();

    // making shared global vector
    msg_queue.resize(n+1);
    process_log.resize(n+1);

    // spawning threads
    std::vector<std::unique_ptr<std::thread>> processes;

    for(int i = 0; i < n; i++)
    {
        std::cout << "Spawning process p" << (i+1) << std::endl;
        processes.emplace_back(std::make_unique<std::thread>(bss_process,i+1,process_events[i]));
    }

    std::cout << "Simulation under progress" << std::endl;

    for(auto &process_thread : processes)
    {
        process_thread.get()->join();
    }

    // dump all
    for(int i=1;i<=n;i++)
    {
        out_file << "begin process p" << i << std::endl;
        for(auto i : process_log[i]){
            out_file << "\t" << i << std::endl;
        }
        out_file << "end process p" << i << "\n" << std::endl;
    }

    // closing files
    in_file.close();
    out_file.close();

    return 0;
}