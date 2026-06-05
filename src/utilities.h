#pragma once //Just ensures the file is only included once
#include <string>

std::string read_string(const std::string& prompt);
int read_integer(const std::string& prompt, int min, int max);
int read_integer(const std::string& prompt);
double read_double(const std::string& prompt);