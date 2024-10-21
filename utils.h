#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include "structs.h"

const std::string begin_process_prefix = "begin process p";
const int begin_p_no_offset = begin_process_prefix.size();

const std::string end_process_prefix = "end process p";
const int end_p_no_offset = end_process_prefix.size();

const std::string send_msg_prefix = "send m";
const int msg_no_offset = send_msg_prefix.size();

const std::string recv_b_prefix = "recv_B";

static bool error_in_input{false};

std::vector<std::vector<event>> parse_commands(std::ifstream &s);

std::vector<std::string> split_by_space(std::string str);