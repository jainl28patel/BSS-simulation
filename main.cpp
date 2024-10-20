#include <iostream>
#include <thread>
#include <fstream>
#include <memory>
#include <vector>
#include <mutex>
#include <set>
#include <unistd.h>
#include "utils.cpp"
#include "main.h"

int n; //no of processes

// global mutex lock for synchronisation
std::mutex global_vector_lock;

// message queue corresponding to each process
std::vector<std::vector<message>> msg_queue;

// list of processes
std::vector<process> p_list;

// std::vector<message> msg_list;

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
    for(auto i : timestamp) std::cout << i << " ";
    std::cout << std::endl;
    for(auto i : p_clock) std::cout << i << " ";
    std::cout << std::endl;
    std::cout << sender_id << std::endl;
    for(int i=0;i<m;i++){
        if(i==sender_id)continue;
        if(p_clock[i]<timestamp[i])return false;
    }
    std::cout << "checked " << std::endl;
    return p_clock[sender_id]==timestamp[sender_id]-1;
}

void process::recieve_message(event &e){
    // std::cout << "recv " << pid << std::endl;
    if(this->recieved_msg.find(e.msg_id)!=this->recieved_msg.end()) return;
    bool is_msg_found=0;
    while(true){
        global_vector_lock.lock();
            // if(msg_queue[pid].size()==0) b
            while(msg_queue[pid].size())
            {
                message curr_msg = msg_queue[pid].back();
                msg_queue[pid].pop_back();

                std::cout << "recv " << curr_msg.sender_id << std::endl;
                if(check_timestamp(curr_msg.timestamp,p_clock,e.sender_pid) && 
                                   curr_msg.msg_id==stoi(e.msg_id.substr(1)) )
                {
                    is_msg_found = true;
                    out_file << "msg BSS_rrecv : " << pid << " " << curr_msg.msg_id << std::endl;
                    out_file << "msg APP_rrecv : " << pid << " " << curr_msg.msg_id << std::endl;
                    this->recieved_msg.insert(curr_msg.string_msg_id);
                        // log bss recv
                        // log app recv

                    // update time
                    for(int i = 0; i < curr_msg.timestamp.size(); i++)
                    {
                        p_clock[i] = std::max(p_clock[i],curr_msg.timestamp[i]);
                    }
                } else {
                    // log bss recv
                    // store in pending msg
                    if(this->expected_msg.find(curr_msg.string_msg_id)!=this->expected_msg.end())
                        out_file << "msg BSS_rrecv : " << pid << " " << curr_msg.msg_id << std::endl;
                    this->pending_msg.push_back(curr_msg);
                }
            }
        global_vector_lock.unlock();

        if(is_msg_found) break;
        else {
            // check pending for new acceptance
            while(true)
            {
                bool koi_mil_gaya = false;
                for(auto i = this->pending_msg.begin(); i!=pending_msg.end(); i++)
                {
                    auto& msg = *i;
                    if(check_timestamp(msg.timestamp, this->p_clock, msg.sender_id)) {
                        koi_mil_gaya = true;
                        // check if curr msg
                        if(msg.msg_id==stoi(e.msg_id.substr(1)))
                        {
                            is_msg_found = true;
                        }
                        // update time
                        for(int i = 0; i < msg.timestamp.size(); i++)
                        {
                            p_clock[i] = std::max(p_clock[i],msg.timestamp[i]);
                        }
                        this->recieved_msg.insert(msg.string_msg_id);
                        // erase
                        // log : deliver to the app
                        if(this->expected_msg.find(msg.string_msg_id)!=this->expected_msg.end())
                            out_file << "msg APP_rrecv : " << pid << " " << msg.msg_id << std::endl;

                        this->pending_msg.erase(i);
                        break;
                    }
                }

                if(!koi_mil_gaya) break;
            }

            if(is_msg_found) break;

            // sleep 5 sec
            // sleep(5);
        }
    }

    // std::cout << "recv done " << pid << std::endl;
}

void process::send_message(event &e){
    message msg;
    std::cout << "msg id 1 :" << e.msg_id << std::endl;
    msg.msg_id=stoi(e.msg_id.substr(1)); // set msg id
    msg.string_msg_id = e.msg_id;
    msg.sender_id=pid;
    p_clock[pid]++;
    msg.timestamp=p_clock;
    global_vector_lock.lock();

    std::cout  << msg.msg_id << " " << msg.sender_id  << std::endl;
    for(auto i : msg.timestamp) std::cout << i << " ";
    std::cout << std::endl;

    for(int id=1;id<=n;id++){
        if(id==pid)continue;
        out_file << "msg send : " << pid << " to : " << id << " " << e.msg_id << std::endl;
        msg_queue[id].push_back(msg);
    }
    global_vector_lock.unlock();
    std::cout << "msg sent " << pid << std::endl;
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

    // spawning threads
    std::vector<std::unique_ptr<std::thread>> processes;

    for(int i = 0; i < n; i++)
    {
        processes.emplace_back(std::make_unique<std::thread>(bss_process,i+1,process_events[i]));
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