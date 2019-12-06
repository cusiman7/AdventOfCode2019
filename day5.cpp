
#include "clue.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <array>

constexpr int ADD           = 1;
constexpr int MULT          = 2;
constexpr int INPUT         = 3;
constexpr int OUTPUT        = 4;
constexpr int JUMP_IF_TRUE  = 5;
constexpr int JUMP_IF_FALSE = 6;
constexpr int LESS_THAN     = 7;
constexpr int EQUALS        = 8;
constexpr int HALT          = 99;

constexpr int POSITION_MODE  = 0;
constexpr int IMMEDIATE_MODE = 1;

int DecodeOpcode(std::vector<int>& memory, int pc, int* parameters[4]) {
    int opcode = memory[pc];
    int op = opcode % 100;
    int n = 0;
    switch (op) {
        case ADD:
        case MULT:
        case LESS_THAN:
        case EQUALS:
            n = 3;
            break;
        case JUMP_IF_TRUE:
        case JUMP_IF_FALSE:
            n = 2;
            break;
        case INPUT:
        case OUTPUT:
            n = 1;
            break;
        default:
            break;
    }
    opcode /= 100;
    for (int i = 0; i < n; i++) {
        int paramMode = opcode % 10;
        switch (paramMode) {
            case POSITION_MODE: 
                parameters[i] = &(memory[memory[pc+i+1]]);
                break;
            case IMMEDIATE_MODE: 
                parameters[i] = &(memory[pc+i+1]);
                break;
        }
        opcode /= 10;
    }
    return op;
}

void RunProgram(std::vector<int>& memory) {
    int pc = 0;
    int* params[4];

    while (true) {
        int op = DecodeOpcode(memory, pc, params);
        switch(op) {
            case ADD:
                *params[2] = *params[0] + *params[1];
                pc += 4;
                break;
            case MULT:
                *params[2] = *params[0] * *params[1];
                pc += 4;
                break;
            case INPUT:
                int input;
                std::cout << "Enter an Integer: ";
                std::cin >> input;
                *params[0] = input;
                pc += 2;
                break;
            case OUTPUT: 
                std::cout << *params[0] << "\n";
                pc += 2;
                break;
            case JUMP_IF_TRUE:
                pc = (*params[0]) ? *params[1] : pc + 3;
                break;
            case JUMP_IF_FALSE: 
                pc = (!*params[0]) ? *params[1] : pc + 3;
                break;
            case LESS_THAN:
                *params[2] = (*params[0] < *params[1]);
                pc += 4;
                break;
            case EQUALS:
                *params[2] = (*params[0] == *params[1]);
                pc += 4;
                break;
            case HALT:
                return;
        }
   }
}

std::vector<int> ReadProgram(std::istream& istream) {
    char comma;
    int opcode;
    std::vector<int> program;
    while (istream.good()) {
        istream >> opcode;
        istream >> comma;
        program.push_back(opcode);
    }
    return program;
}

struct Args {
    std::string file = "day5.txt";
    std::string test = "";
};

int main(int argc, char** argv) {
    clue::CommandLine<Args> cl;
    cl.Optional(&Args::test, "test");
    cl.Positional(&Args::file, "file");
    auto args = cl.ParseArgs(argc, argv);

    if (!args->test.empty()) {
        std::istringstream in(args->test);
        std::vector<int> program = ReadProgram(in);
        RunProgram(program);
    } else if (!args->file.empty()) {
        std::ifstream file(args->file);
        std::vector<int> program = ReadProgram(file);
        RunProgram(program);
    }
}
