
#include "clue.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <array>
#include <algorithm>

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

bool RunProgram(std::vector<int>& memory, int& pc, std::deque<int>* inputs, std::deque<int>* outputs) {
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
                if (inputs->empty()) {
                    return false;
                }
                *params[0] = inputs->front();
                inputs->pop_front();
                pc += 2;
                break;
            case OUTPUT: 
                outputs->push_back(*params[0]);
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
                return true;
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

struct Amplifier {
    std::vector<int> program;
    int pc = 0;
    std::deque<int> inputs;
    std::deque<int>* outputs;
    bool halted = false;
};

struct Args {
    std::string file = "day7.txt";
    std::string test = "";
    bool part2 = false;
};

int main(int argc, char** argv) {
    clue::CommandLine<Args> cl;
    cl.Optional(&Args::test, "test");
    cl.Optional(&Args::part2, "part2");
    cl.Positional(&Args::file, "file");
    auto args = cl.ParseArgs(argc, argv);

    std::vector<int> program;

    if (!args->test.empty()) {
        std::istringstream in(args->test);
        program = ReadProgram(in);
    } else if (!args->file.empty()) {
        std::ifstream file(args->file);
        program = ReadProgram(file);
    }

    if (!args->part2) {
        std::array<int, 5> phaseSettings = {0, 1, 2, 3, 4};
        int maxSignal = 0;
        while (std::next_permutation(phaseSettings.begin(), phaseSettings.end())) {
            std::deque<int> outputs;
            std::array<Amplifier, 5> amps;
            for(int i = 0; i < 5; i++) {
                amps[i].program = program;
                amps[i].pc = 0;
                if (i != 4) {
                    amps[i].outputs = &amps[i+1].inputs;
                } else {
                    amps[4].outputs = &outputs;
                }
                amps[i].inputs.push_back(phaseSettings[i]);
            }
            amps[0].inputs.push_back(0);
            for(int i = 0; i < 5; i++) {
                RunProgram(amps[i].program, amps[i].pc, &amps[i].inputs, amps[i].outputs);
            }
            maxSignal = std::max(maxSignal, outputs.back());
        }
        std::cout << maxSignal << "\n";
    } else {
        std::array<int, 5> phaseSettings = {5, 6, 7, 8, 9};
        int maxSignal = 0;
        while (std::next_permutation(phaseSettings.begin(), phaseSettings.end())) {
            std::array<Amplifier, 5> amps;
            for(int i = 0; i < 5; i++) {
                amps[i].program = program;
                amps[i].pc = 0;
                if (i != 4) {
                    amps[i].outputs = &amps[i+1].inputs;
                } else {
                    amps[4].outputs = &amps[0].inputs;
                }
                amps[i].inputs.push_back(phaseSettings[i]);
                amps[i].halted = false;
            }
            amps[0].inputs.push_back(0);
            int i = 0;
            while (true) {
                amps[i].halted = RunProgram(amps[i].program, amps[i].pc, &amps[i].inputs, amps[i].outputs);
                if (std::all_of(amps.begin(), amps.end(), [](const auto& a) { return a.halted; })) break;
                i = ++i % 5;
            }
            maxSignal = std::max(maxSignal, amps[4].outputs->back());
        }
        std::cout << maxSignal << "\n";

    }
}

