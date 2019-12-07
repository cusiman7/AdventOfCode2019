
#include "clue.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <deque>

struct Object {
    std::string name;
    Object* center = nullptr;
    std::vector<Object*> outerObjects;
};

using OrbitsMap = std::unordered_map<std::string, Object*>;

OrbitsMap ReadOrbits(std::istream& istream) {
    OrbitsMap objects; 
    while (istream.good()) {
        std::string token;
        istream >> token;
        size_t mid = token.find_first_of(")");
        std::string centerName = token.substr(0, mid);
        std::string objectName = token.substr(mid+1);

        Object* center = nullptr;
        auto centerIt = objects.find(centerName);
        if (centerIt == objects.end()) {
            center = new Object();
            center->name = centerName;
            objects.insert({centerName, center});
        } else {
            center = centerIt->second;
        }
        Object* o = nullptr;
        auto objectIt = objects.find(objectName);
        if (objectIt == objects.end()) {
            o = new Object();
            o->name = objectName;
            objects.insert({objectName, o});
        } else {
            o = objectIt->second;
        }
        o->center = center;
        center->outerObjects.push_back(o);
    }
    return objects;
}

int CountOrbits(Object* o, int distanceFromCOM) {
    int orbitCount = distanceFromCOM;
    for (Object* outerObject : o->outerObjects) {
        orbitCount += CountOrbits(outerObject, distanceFromCOM + 1); 
    }
    return orbitCount; 
}

std::deque<Object*> RootToObject(Object* o) {
    std::deque<Object*> fromRoot;
    Object* curr = o;
    while (curr) {
        fromRoot.push_front(curr);
        curr = curr->center;
    }
    return fromRoot;
}

int OrbitalTransfers(Object* a, Object* b) {
    auto rootToA = RootToObject(a);
    auto rootToB = RootToObject(b);
    
    int i = 0;
    while (rootToA[i] == rootToB[i]) {
        i++;
    }
    return rootToA.size() - i + rootToB.size() - i;
}

struct Args {
    std::string file = "day6.txt";
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
        OrbitsMap orbits = ReadOrbits(in);
        if (!args->part2) {
            int orbitsCount = CountOrbits(orbits["COM"], 0);
            std::cout << orbitsCount << "\n";
        } else {
            int transfers = OrbitalTransfers(orbits["YOU"]->center, orbits["SAN"]->center);
            std::cout << transfers << "\n";
        }
    } else if (!args->file.empty()) {
        std::ifstream file(args->file);
        OrbitsMap orbits = ReadOrbits(file);
        if (!args->part2) {
            int orbitsCount = CountOrbits(orbits["COM"], 0);
            std::cout << orbitsCount << "\n";
        } else {
            int transfers = OrbitalTransfers(orbits["YOU"]->center, orbits["SAN"]->center);
            std::cout << transfers << "\n";
        }
    }
}

