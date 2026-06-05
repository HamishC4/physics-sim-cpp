#include "utilities.h"
#include <iostream>
#include <string>
#include <limits>
//Read string
std::string read_string(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

//Read an integer between min and max
int read_integer(const std::string& prompt, int min, int max) {
    while (true) {
        std::string input = read_string(prompt);
        try {
            int val = std::stoi(input);
            if (val >= min && val <= max) return val;
            std::cout << "Please enter an integer between " << min << " and " << max << "\n";
        } catch (...) {
            std::cout << "Please enter an integer between " << min << " and " << max << "\n";
        }
    }
}

//Read integer
int read_integer(const std::string& prompt) {
    while (true) {
        std::string input = read_string(prompt);
        try {
            return std::stoi(input);
        } catch (...) {
            std::cout << "Please enter an integer!\n";
        }
    }
}

//Read double
double read_double(const std::string& prompt) {
    while (true) {
        std::string input = read_string(prompt);
        try {
            return std::stod(input);
        } catch (...) {
            std::cout << "Please enter a number!\n";
        }
    }
}