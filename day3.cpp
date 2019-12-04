
#include "clue.h"
#include <fstream>
#include <iostream>
#include <sstream>

struct Point {
    int x, y;
};

using Wire = std::vector<Point>;

Wire ReadOneWire(std::istream& istream) {
    char comma;
    char dir;
    int dist;
    std::vector<Point> wire;
    wire.push_back({0, 0});
    while (istream.good()) {
        Point curr = wire.back();
        istream >> dir;
        istream >> dist;
        istream >> comma;
        switch (dir) {
            case 'U':
                curr.y += dist;
                break;
            case 'R':
                curr.x += dist;
                break;
            case 'D':
                curr.y -= dist;
                break;
            case 'L':
                curr.x -= dist;
                break; 
        }
        wire.push_back(curr);
    }
    return wire;
}

std::pair<Wire, Wire> ReadTwoWires(std::istream& istream) {
    std::string line1, line2;
    std::getline(istream, line1);
    std::getline(istream, line2);

    std::stringstream ss(line1);
    Wire wire1 = ReadOneWire(ss);
    ss.str(line2);
    ss.clear();
    Wire wire2 = ReadOneWire(ss);

    return {wire1, wire2};
}

Point Intersection(Point a0, Point a1, Point b0, Point b1) {
    if (a0.x == a1.x && b0.y == b1.y) {
        if (a0.y > a1.y) {
            return Intersection(a1, a0, b0, b1);
        }
        if (b0.x > b1.x) {
            return Intersection(a0, a1, b1, b0);
        }
        if (a0.y <= b0.y && a1.y >= b1.y && b0.x <= a0.x && b1.x >= a1.x) {
            return {a0.x, b0.y};
        }
        return {0, 0};
    } else if (a0.y == a1.y && b0.x == b1.x) {
        return Intersection(b0, b1, a0, a1);
    } else {
        return {0, 0};
    }
}

int ClosestIntersection(Wire a, Wire b) {
    int shortestDistance = std::numeric_limits<int>::max();
    for (size_t i = 0; i < a.size()-1; i++) {
        Point a0 = a[i];
        Point a1 = a[i+1]; 
        
        for (size_t j = 0; j < b.size()-1; j++) {
            Point b0 = b[j];
            Point b1 = b[j+1];
    
            Point p = Intersection(a0, a1, b0, b1);
            if (p.x != 0 && p.y != 0) {
                shortestDistance = std::min(shortestDistance, std::abs(p.x) + std::abs(p.y));
            }
        } 
    }
    return shortestDistance;
}

int FewestCombinedSteps(Wire a, Wire b) {
    int aSteps = 0;
    for (size_t i = 0; i < a.size()-1; i++) {
        Point a0 = a[i];
        Point a1 = a[i+1]; 

        int bSteps = 0;
        for (size_t j = 0; j < b.size()-1; j++) {
            Point b0 = b[j];
            Point b1 = b[j+1];
    
            Point p = Intersection(a0, a1, b0, b1);
            int aStepsToP = std::abs(p.x - a0.x) + std::abs(p.y - a0.y);
            int bStepsToP = std::abs(p.x - b0.x) + std::abs(p.y - b0.y);
            if (p.x != 0 && p.y != 0) {
                return aSteps + bSteps + aStepsToP + bStepsToP; 
            }
            bSteps += std::abs(b1.x - b0.x) + std::abs(b1.y - b0.y);
        } 
        aSteps += std::abs(a1.x - a0.x) + std::abs(a1.y - a0.y);
    }
    return -1;
}

struct Args {
    std::string file = "day3.txt";
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
        auto [a, b] = ReadTwoWires(in);
        if (!args->part2) {
            int shortestDistance = ClosestIntersection(a, b);
            std::cout << shortestDistance << "\n";
        } else {
            int steps = FewestCombinedSteps(a, b);
            std::cout << steps << "\n";
        }
    } else if (!args->file.empty()) {
        std::ifstream in(args->file);
        auto [a, b] = ReadTwoWires(in);
        if (!args->part2) {
            int dist = ClosestIntersection(a, b);
            std::cout << dist << "\n";
        } else {
            int steps = FewestCombinedSteps(a, b);
            std::cout << steps << "\n";
        }
    }
}

