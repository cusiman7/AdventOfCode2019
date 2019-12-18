
#include "clue.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

constexpr int64_t ADD           = 1;
constexpr int64_t MULT          = 2;
constexpr int64_t INPUT         = 3;
constexpr int64_t OUTPUT        = 4;
constexpr int64_t JUMP_IF_TRUE  = 5;
constexpr int64_t JUMP_IF_FALSE = 6;
constexpr int64_t LESS_THAN     = 7;
constexpr int64_t EQUALS        = 8;
constexpr int64_t RELATIVE_ADJ  = 9;
constexpr int64_t HALT          = 99;

constexpr int64_t POSITION_MODE  = 0;
constexpr int64_t IMMEDIATE_MODE = 1;
constexpr int64_t RELATIVE_MODE  = 2;

enum Interrupt {
    kInput,
    kHalt
};

struct Intcode {
    std::vector<int64_t> memory;
    int64_t pc = 0;
    int64_t relativeBase = 0;
};

int64_t DecodeOpcode(Intcode& intcode, int64_t* parameters[4]) {
    int64_t opcode = intcode.memory[intcode.pc];
    int64_t op = opcode % 100;
    int64_t n = 0;
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
        case RELATIVE_ADJ:
            n = 1;
            break;
        default:
            break;
    }
    opcode /= 100;
    for (int64_t i = 0; i < n; i++) {
        int64_t paramMode = opcode % 10;
        switch (paramMode) {
            case POSITION_MODE: 
                parameters[i] = &(intcode.memory[intcode.memory[intcode.pc+i+1]]);
                break;
            case IMMEDIATE_MODE: 
                parameters[i] = &(intcode.memory[intcode.pc+i+1]);
                break;
            case RELATIVE_MODE:
                parameters[i] = &(intcode.memory[intcode.relativeBase + intcode.memory[intcode.pc+i+1]]);
                break;
        }
        opcode /= 10;
    }
    return op;
}

Interrupt RunProgram(Intcode& intcode, std::deque<int64_t>* inputs = nullptr, std::deque<int64_t>* outputs = nullptr) {
    int64_t* params[4];

    while (true) {
        int64_t op = DecodeOpcode(intcode, params);
        switch(op) {
            case ADD:
                *params[2] = *params[0] + *params[1];
                intcode.pc += 4;
                break;
            case MULT:
                *params[2] = *params[0] * *params[1];
                intcode.pc += 4;
                break;
            case INPUT:
                if (inputs) {
                    if (inputs->empty()) {
                        return kInput;
                    }
                    *params[0] = inputs->front();
                    inputs->pop_front();
                } else {
                    std::cout << "Enter an Integer: ";
                    std::cin >> (*params[0]);
                }
                intcode.pc += 2;
                break;
            case OUTPUT: 
                if (outputs) {
                    outputs->push_back(*params[0]);
                } else {
                    std::cout << *params[0] << "\n";
                }
                intcode.pc += 2;
                break;
            case JUMP_IF_TRUE:
                intcode.pc = (*params[0]) ? *params[1] : intcode.pc + 3;
                break;
            case JUMP_IF_FALSE: 
                intcode.pc = (!*params[0]) ? *params[1] : intcode.pc + 3;
                break;
            case LESS_THAN:
                *params[2] = (*params[0] < *params[1]);
                intcode.pc += 4;
                break;
            case EQUALS:
                *params[2] = (*params[0] == *params[1]);
                intcode.pc += 4;
                break;
            case RELATIVE_ADJ:
                intcode.relativeBase += *params[0]; 
                intcode.pc += 2;
                break;
            case HALT:
                return kHalt;
        }
    }
}

std::vector<int64_t> ReadProgram(std::istream& istream) {
    char comma;
    int64_t opcode;
    std::vector<int64_t> program;
    while (istream.good()) {
        istream >> opcode;
        istream >> comma;
        program.push_back(opcode);
    }
    return program;
}

void DrawScreen(int screen[256][256], int score, int maxX, int maxY) {
    printf("SCORE: %d\n", score);
    for (int y = 0; y <= maxY; y++) {
        for (int x = 0; x <= maxX; x++) {
            switch (screen[y][x]) {
                case 0:
                    printf(" ");
                    break;
                case 1:
                    printf("|");
                    break;
                case 2:
                    printf("#");
                    break;
                case 3:
                    printf("_");
                    break;
                case 4:
                    printf("o");
                    break;
            }
        }
        printf("\n");
    }
}

struct Args {
    std::string file = "day13.txt";
    std::string test = "";
    bool part2 = false;
};

int main(int argc, char** argv) {
    clue::CommandLine<Args> cl;
    cl.Optional(&Args::test, "test");
    cl.Optional(&Args::part2, "part2");
    cl.Positional(&Args::file, "file");
    auto args = cl.ParseArgs(argc, argv);

    Intcode intcode;

    if (!args->test.empty()) {
        std::istringstream in(args->test);
        intcode.memory = ReadProgram(in);
    } else if (!args->file.empty()) {
        std::ifstream file(args->file);
        intcode.memory = ReadProgram(file);
    }
    intcode.memory.resize(intcode.memory.size() * 10, 0);
    if (args->part2) {
        intcode.memory[0] = 2;
    } 

    std::deque<int64_t> inputs;
    std::deque<int64_t> outputs;
    int screen[256][256] = {0};
    int maxX = 0;
    int maxY = 0;
    int score = 0;

    int ballX = 0;
    int paddleX = 0;

    while (true) {
        Interrupt interrupt = RunProgram(intcode, &inputs, &outputs);
        while (!outputs.empty()) {
            int x = outputs.front(); outputs.pop_front();
            int y = outputs.front(); outputs.pop_front();
            int tileId = outputs.front(); outputs.pop_front();
            if (x == -1 && y == 0) {
                score = tileId;
            } else {
                screen[y][x] = tileId;
            }
            
            if (tileId == 3) {
                paddleX = x;   
                screen[y][x] = tileId;
            } else if (tileId == 4) {
                ballX = x;
                screen[y][x] = tileId;
            }

            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);
        }
        DrawScreen(screen, score, maxX, maxY);
        if (interrupt == kHalt) break;
        if (paddleX > ballX) inputs.push_back(-1);
        if (paddleX < ballX) inputs.push_back(1);
        if (paddleX == ballX) inputs.push_back(0);
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(16ms);
    }
}

