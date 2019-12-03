
#include "clue.h"
#include <fstream>
#include <iostream>
#include <sstream>

void RunProgram(std::vector<int>& memory) {
    int pc = 0;
    int opcode = memory[pc];
    while (true) {
        switch(opcode) {
            case 1:
                memory[memory[pc+3]] = memory[memory[pc+1]] + memory[memory[pc+2]];
                break;
            case 2:
                memory[memory[pc+3]] = memory[memory[pc+1]] * memory[memory[pc+2]];
                break;
            case 99:
                return;
        }
        pc += 4;
        opcode = memory[pc];
   }
}

int SearchForNounVerb(std::vector<int> memory) {
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            std::vector<int> memCopy = memory;
            int noun = i;
            int verb = j;
            memCopy[1] = noun;
            memCopy[2] = verb;
            RunProgram(memCopy);
            if (memCopy[0] == 19690720) {
                return (100 * noun) + verb;
            }
        }
    }
    return -1;
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
    std::string file = "day2.txt";
    std::string test = "";
    bool part2 = false;
};

int main(int argc, char** argv) {
    clue::CommandLine<Args> cl;
    cl.Optional(&Args::test, "test");
    cl.Optional(&Args::part2, "part2");
    cl.Positional(&Args::file, "file");
    auto args = cl.ParseArgs(argc, argv);

    if (!args->test.empty()) {
        std::istringstream in(args->test);
        std::vector<int> program = ReadProgram(in);
        RunProgram(program);
        for (int i : program) {
            std::cout << i << ",";
        }
        std::cout << "\n";
    } else if (!args->file.empty()) {
        std::ifstream file(args->file);
        std::vector<int> program = ReadProgram(file);
        if (!args->part2) {
            program[1] = 12;
            program[2] = 2;
            RunProgram(program);
            std::cout << program[0] << "\n";
        } else {
            int result = SearchForNounVerb(program);
            std::cout << result << "\n";
        }
    }
}
