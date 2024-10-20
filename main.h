#pragma once
#include<string>
#include<vector>
#include<queue>
#include "structs.cpp"

struct message{
    int msg_id;
    int sender_id;
    message();
    std::vector<int> timestamp;
};

struct process{   
    int pid;
    std::vector<int> p_clock;
    void recieve_message(event &e);
    void send_message(event &e);
    void run(std::vector<event> &instructions);
};

main.h
