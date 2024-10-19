#include "utils.h"
#include <assert.h>
#include <stdlib.h>

std::vector<std::vector<event>> parse_commands(std::ifstream &s)
{
    std::string file_line;
    std::vector<event> process_events{};
    std::vector<std::vector<event>> all_process_events{};
    int curr_pid;
    int curr_p_eid;
    std::string curr_s_pid;
    while (std::getline(s, file_line))
    {
        if (file_line.starts_with(begin_process_prefix))
        {
            assert(process_events.size() == 0);
            curr_pid = std::stoi(file_line.substr(begin_p_no_offset));
            curr_s_pid = file_line.substr(begin_p_no_offset - 1);
            curr_p_eid = 0;
        }
        else if (file_line.starts_with(end_process_prefix))
        {
            if (process_events.size())
            {
                all_process_events.push_back(process_events);
            }
            process_events.clear();
            assert(process_events.size() == 0);
        }
        else if (file_line.starts_with(send_msg_prefix))
        {
            int curr_msg_id = std::stoi(file_line.substr(msg_no_offset));
            process_events.push_back(event(true, curr_pid, curr_p_eid, curr_s_pid, file_line.substr(msg_no_offset - 1), curr_pid, curr_s_pid));
            curr_p_eid++;
        }
        else if (file_line.starts_with(recv_b_prefix))
        {
            std::vector<std::string> curr_line_parts = split_by_space(file_line);
            int msg_id = std::stoi(curr_line_parts[2].substr(1));
            int msg_sender_pid = std::stoi(curr_line_parts[1].substr(1));
            process_events.push_back(event(false, curr_pid, curr_p_eid, curr_s_pid, curr_line_parts[2], msg_sender_pid, curr_line_parts[1]));
            curr_p_eid++;
        }
    }
    return all_process_events;
}

std::vector<std::string> split_by_space(std::string str)
{
    if (!str.size())
        return {};
    std::string curr;
    std::vector<std::string> parts;
    for (char c : str)
    {
        if (c == ' ')
        {
            if (curr.size())
            {
                parts.push_back(curr);
            }
            curr = "";
        }
        else
        {
            curr.push_back(c);
        }
    }
    if (curr.size())
        parts.push_back(curr);
    return parts;
}