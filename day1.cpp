
#include "clue.h"
#include <fstream>
#include <iostream>

int RocketFuel(int mass) { 
    return static_cast<int>(std::floor((float)mass / 3.0f)) - 2;
}

int RocketFuel2(int mass) { 
    int fuel = static_cast<int>(std::floor((float)mass / 3.0f)) - 2;
    if (fuel <= 0) return 0;
    return fuel + RocketFuel2(fuel);
}

struct Args {
    std::string file = "day1.txt";
    int test = 0;
    bool part2 = false;
};

int main(int argc, char** argv) {

    clue::CommandLine<Args> cl;
    cl.Optional(&Args::test, "test");
    cl.Optional(&Args::part2, "part2");
    cl.Positional(&Args::file, "file");
    auto args = cl.ParseArgs(argc, argv);
    
    if (args->test != 0) {
        std::cout << (args->part2 ? RocketFuel2(args->test) : RocketFuel(args->test)) << "\n";
    } else if (!args->file.empty()) {
        std::ifstream file(args->file);
        int input = 0;
        int totalFuel = 0;
        file >> input;
        do {
            totalFuel += args->part2 ? RocketFuel2(input) : RocketFuel(input);
            file >> input;
        } while(file.good());
        std::cout << totalFuel << "\n";
    }
}
