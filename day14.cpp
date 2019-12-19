
#include "clue.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <set>

struct Reaction {
    std::unordered_map<std::string, int64_t> inputs;
    std::string name;
    int64_t amount;
};
using ReactionsMap = std::unordered_map<std::string, Reaction>;

ReactionsMap ReadReactions(std::istream& istream) {
    ReactionsMap reactions;

    while (istream.good()) {
        std::string line;
        std::getline(istream, line);
        if (line.empty()) {
            break;
        }
        std::stringstream ss(line);
        std::string delim;
        Reaction reaction;
        do {
            int amount;
            std::string name;
            ss >> amount;
            ss >> name;
            size_t i = name.find_first_of(",");
            if (i != std::string::npos) {
                name = name.substr(0, i);
                reaction.inputs.insert({name, amount});
            } else {
                reaction.inputs.insert({name, amount});
                std::string delim;
                ss >> delim;
                break;
            }
        } while (true);
        
        ss >> reaction.amount;
        ss >> reaction.name;
        reactions.insert({reaction.name, reaction});
    }
    return reactions;
}

int64_t OrePerFuel(const ReactionsMap& reactions, int64_t fuel) {
    std::unordered_map<std::string, int64_t> inputAvailable;
    std::set<std::string> inputsNeeded;
    inputsNeeded.insert("FUEL");
    inputAvailable.insert({"FUEL", -1 * fuel});

    while(!inputsNeeded.empty()) {
        auto inputName = *inputsNeeded.begin();
        inputsNeeded.erase(inputName);

        // No reacton produces ORE
        if (inputName == "ORE") continue;

        const auto& reaction = reactions.at(inputName);
        int64_t deficit = inputAvailable[inputName];
        if (deficit > 0) continue;
        int64_t multiplier = std::ceil(std::abs(deficit) / (double)reaction.amount);
        // "Run" the reaction if needed
        for (const auto& [name, amount] : reaction.inputs) {
            inputAvailable[name] -= amount * multiplier;
            inputsNeeded.insert(name);
        }
        inputAvailable[inputName] += reaction.amount * multiplier;
    }
    return std::abs(inputAvailable["ORE"]);
}

struct Args {
    std::string file = "day14.txt";
};

int main(int argc, char** argv) {

    clue::CommandLine<Args> cl;
    cl.Positional(&Args::file, "file");
    auto args = cl.ParseArgs(argc, argv);

    ReactionsMap reactions;
    
    if (!args->file.empty()) {
        std::ifstream in(args->file);
        reactions = ReadReactions(in);
    }
    int64_t fuel = 1;
    int64_t oreRequired = OrePerFuel(reactions, fuel); 
    printf("%lld ORE required for 1 FUEL\n", oreRequired);

    const int64_t ore = 1000000000000;
    while (true) {
        oreRequired = OrePerFuel(reactions, fuel + 1);
        if (oreRequired > ore) {
            break;
        } else {
            fuel = std::max(fuel+1, (int64_t)std::floor((fuel+1) * (double)ore / oreRequired));
        }
    }
    printf("FUEL made %lld\n", fuel);
}

