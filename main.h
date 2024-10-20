#pragma once
#include<string>
#include<vector>
#include<queue>
#include<set>
#include "structs.cpp"

struct message{
    int msg_id;
    std::string string_msg_id;
    int sender_id;
    std::vector<int> timestamp;
    message();
    message(message const& msg) {
        this->msg_id = msg.msg_id;
        this->sender_id = msg.sender_id;
        this->string_msg_id = msg.string_msg_id;
        for(auto i : msg.timestamp) {
            this->timestamp.push_back(i);
        }
    }
};

struct process{   
    int pid;
    std::vector<int> p_clock;
    std::set<std::string> recieved_msg;
    std::set<std::string> expected_msg;
    std::vector<message> pending_msg;
    process(int id, int n) : pid(id), p_clock(std::vector<int>(n+1,0)) {}
    void recieve_message(event &e);
    void send_message(event &e);
    void run(std::vector<event> &instructions);
};

