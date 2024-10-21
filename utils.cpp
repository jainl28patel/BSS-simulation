#include "utils.h"
#include <assert.h>
#include <stdlib.h>
#include <set>
#include <fstream>
#include <iostream>

std::vector<std::vector<event>> parse_commands(std::ifstream &s)
{
    std::string file_line;
    std::vector<event> process_events{};
    std::vector<std::vector<event>> all_process_events{};
    int curr_pid;
    int curr_p_eid;
    std::string curr_s_pid;
    std::set<std::string> sent_msgs, recv_msgs;
    while (std::getline(s, file_line))
    {
        while (file_line.length() && !isalnum(file_line.back()))
            file_line.pop_back();
        if (file_line == "")
            continue; // empty line
        if (file_line.starts_with(begin_process_prefix))
        {
            assert(process_events.size() == 0);
            curr_s_pid = file_line.substr(begin_p_no_offset - 1);
            if (curr_s_pid.size() != 2 || curr_s_pid[0] != 'p' || curr_s_pid[1] > '9' || curr_s_pid[1] < '0')
            {
                std::ofstream result_file;
                result_file.open("./results/output.txt");
                result_file << "Exited after detecting an error in input file\nError Type: Invalid command\n";
                result_file << "Error details:\n\tWrong/no process number in a begin process command\n\tError causing line is: " << file_line;
                result_file.close();
                error_in_input = true;
                return {};
            }
            curr_pid = std::stoi(curr_s_pid.substr(1));
            curr_p_eid = 0;
        }
        else if (file_line.starts_with(end_process_prefix))
        {
            if (file_line.size() != end_p_no_offset + 1 || file_line.at(end_p_no_offset) < '0' || file_line.at(end_p_no_offset) > '9')
            {
                std::ofstream result_file;
                result_file.open("./results/output.txt");
                result_file << "Exited after detecting an error in input file\nError Type: Invalid command\n";
                result_file << "Error details:\n\tWrong/no process number in an end process command\n\tError causing line is: " << file_line;
                result_file.close();
                error_in_input = true;
                return {};
            }
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
            sent_msgs.insert(curr_s_pid + " " + file_line.substr(msg_no_offset - 1));
        }
        else if (file_line.starts_with(recv_b_prefix))
        {
            std::vector<std::string> curr_line_parts = split_by_space(file_line);
            if (curr_line_parts.size() != 3)
            {
                // error - invalid recvb command
                std::ofstream result_file;
                result_file.open("./results/output.txt");
                result_file << "Exited after detecting an error in input file\nError Type: Invalid recv_B command\n";
                result_file << "Error details:\n\tWrong number of args in a recv_B command\n\tError causing command: " << file_line;
                result_file.close();
                error_in_input = true;
                return {};
            }
            else if (curr_line_parts[1].size() != 2 || curr_line_parts[2][0] != 'm' || curr_line_parts[2].size() == 1)
            {
                // error - invalid process / msdg id in recvb command
                std::ofstream result_file;
                result_file.open("./results/output.txt");
                result_file << "Exited after detecting an error in input file\nError Type: Invalid recv_B command\n";
                result_file << "Error details:\n\tInvalid process/msg id in a recv_B command\n\tError causing line is: " << file_line;
                result_file.close();
                error_in_input = true;
                return {};
            }
            int msg_id = std::stoi(curr_line_parts[2].substr(1));
            int msg_sender_pid = std::stoi(curr_line_parts[1].substr(1));
            process_events.push_back(event(false, curr_pid, curr_p_eid, curr_s_pid, curr_line_parts[2], msg_sender_pid, curr_line_parts[1]));
            curr_p_eid++;
            recv_msgs.insert(curr_line_parts[1] + " " + curr_line_parts[2]);
        }
        else
        {
            // invalid command
            std::ofstream result_file;
            result_file.open("./results/output.txt");
            result_file << "Exited after detecting an error in input file\nError Type: Invalid command\n";
            result_file << "Error details:\n\tError causing command line: " << file_line;
            result_file.close();
            error_in_input = true;
            return {};
        }
    }
    // checking
    for (auto rcvd_msg : recv_msgs)
    {
        if (sent_msgs.find(rcvd_msg) == sent_msgs.cend())
        {
            // flag error
            // write in the error file
            std::ofstream result_file;
            result_file.open("./results/output.txt");
            result_file << "Exited after detecting an error in input file\nError Type: A msg that was received in one process was never sent by another\n";
            result_file << "Error details:\n\terror_in_input msg received: " << rcvd_msg;
            result_file.close();
            error_in_input = true;
            return {};
        }
    }
    return all_process_events;
}

std::vector<std::string> split_by_space(std::string str)
{
    if (!str.size())
    {
        error_in_input = true;
        return {};
    }
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