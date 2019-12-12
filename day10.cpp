
#include "clue.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <numeric>
#include <thread>

using AsteroidMap = std::vector<std::vector<char>>;
struct Point {
    int x;
    int y;
};

AsteroidMap ReadAsteroidMap(std::istream& istream) {
    AsteroidMap map;

    while (istream.good()) {
        std::string line;
        std::getline(istream, line);
        if (line.empty()) {
            break;
        }
        map.emplace_back(0);
        for (char c : line) {
            map.back().push_back(c);
        }
    }
    return map;
}

Point SlopeBetweenPoints(Point a, Point b) {
    int dx = b.x - a.x;
    int dy = b.y - a.y;

    int gcd = std::gcd(dx, dy);
    dx /= gcd;
    dy /= gcd;

    return {dx, dy};
}

bool Occluded(const AsteroidMap& map, Point source, Point target) {
    int width = map.back().size();
    int height = map.size();

    auto [dx, dy] = SlopeBetweenPoints(source, target);
    int x = source.x + dx;
    int y = source.y + dy;
    bool occluded = false;
    while (true) {
        if (y < 0 || x < 0 || y >= height || x >= width || (x == target.x && y == target.y)) {
            break;
        }
        if (map[y][x] == '#') {
            occluded = true;
            break;
        }
        x += dx;
        y += dy;
    }
    return occluded;
}

int VisibleAsteroids(const AsteroidMap& map, int x, int y) {
    int width = map.back().size();
    int height = map.size();
    int visibleAsteroids = 0;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (i == x && j == y) continue;
            if (map[j][i] == '.') continue;

            bool occluded = Occluded(map, {x, y}, {i, j});
            if (!occluded) {
                visibleAsteroids++;
            }
        }
    }
    return visibleAsteroids;
}

std::vector<Point> SortedAsteroidsByAngle(const AsteroidMap& map, Point source) {
    int width = map.back().size();
    int height = map.size();

    std::vector<Point> sortedAsteroids;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == '#') sortedAsteroids.push_back({x, y}); 
        }
    }

    std::sort(sortedAsteroids.begin(), sortedAsteroids.end(), [source](const Point& a, const Point& b) {
        const float pi = 3.14159265359;
        float dx = a.x - source.x;
        float dy = a.y - source.y;

        float angleA = atan2(dy, dx);
        
        dx = b.x - source.x;
        dy = b.y - source.y;

        float angleB = atan2(dy, dx);

        if (angleA < -pi/2 && angleB < -pi/2) {
            return angleA < angleB;
        }
        if (angleA < -pi/2 && angleB > -pi/2) {
            return false;
        }
        if (angleA > -pi/2 && angleB < -pi/2) {
            return true;
        }
        return angleA < angleB;
    });
    return sortedAsteroids;
}

void PrintMap(const AsteroidMap& map) {
    for (auto& l : map) {
        for (auto c : l) {
            std::cout << c;
        }
        std::cout << "\n";
    }
}

void IMAFIRINMAHLAZER(AsteroidMap map, std::vector<Point> sortedAsteroids, Point source) {
    int count = 0;
    Point luckyTwoHundred;
    while (count < 200) {
        float previousAngle = -100;
        for (auto& p : sortedAsteroids) {
            if (map[p.y][p.x] != '#') continue;

            float dx = p.x - source.x;
            float dy = p.y - source.y;
            float angle = atan2(dy, dx);
            if (angle == previousAngle) {
                continue;
            }

            bool occluded = Occluded(map, source, p);
            if (!occluded) {
                previousAngle = angle;
                map[p.y][p.x] = '*';
                PrintMap(map);
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(0.1s);
                count++;
                if (count == 200) {
                    luckyTwoHundred = p;
                    break;
                }
            }
        } 
    }
    std::cout << "200th asteroid was " << luckyTwoHundred.x << ", " << luckyTwoHundred.y << "\n";
    std::cout << luckyTwoHundred.x * 100 + luckyTwoHundred.y << "\n";
}

struct Args {
    std::string file = "day10.txt";
    std::string test = "";
    bool part2 = false;
};

int main(int argc, char** argv) {

    clue::CommandLine<Args> cl;
    cl.Optional(&Args::part2, "part2");
    cl.Positional(&Args::file, "file");
    auto args = cl.ParseArgs(argc, argv);

    AsteroidMap map;
    
    if (!args->test.empty()) {
        std::stringstream in(args->test);
        map = ReadAsteroidMap(in);
    } else if (!args->file.empty()) {
        std::ifstream in(args->file);
        map = ReadAsteroidMap(in);
    }

    int maxVisibleAsteroids = 0;
    int x, y;

    int width = map.back().size();
    int height = map.size();
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (map[j][i] == '.') continue;
            int visible = VisibleAsteroids(map, i, j);
            if (visible > maxVisibleAsteroids) {
                maxVisibleAsteroids = visible;
                x = i;
                y = j;
            }
        }
    }
    std::cout << "Best location (" << x << ", " << y << ") can see " << maxVisibleAsteroids << " asteroids\n";
    map[y][x] = 'X';

    auto sortedAsteroids = SortedAsteroidsByAngle(map, {x, y});
    IMAFIRINMAHLAZER(map, sortedAsteroids, {x, y});
}

