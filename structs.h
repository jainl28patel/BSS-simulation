#include <fstream>

#pragma once

struct event
{
    bool is_send_event;
    int pid;
    int p_eid;
    std::string s_pid;
    std::string msg_id;
    int sender_pid;
    std::string str_sender_pid;
};

std::ostream &operator<<(std::ostream &s, event &e);