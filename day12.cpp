
#include "clue.h"
#include <algorithm>
#include <unordered_set>
#include <numeric>

struct Vec3 {
    int x, y, z;
};
Vec3 operator+(const Vec3& a, const Vec3& b) {
    Vec3 v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    v.z = a.z + b.z;
    return v;
}

enum Moons {
    Io,
    Europa,
    Ganymede,
    Callisto
};

struct Moon {
    Moons moon; 
    Vec3 pos;
    Vec3 vel;
};

void Gravity(const int& aPos, const int& bPos, int& aVel, int& bVel) {
    if (aPos > bPos) {
        bVel++;
        aVel--;
    } else if (bPos > aPos) {
        aVel++;
        bVel--;
    }
}

int Energy(const Moon& moon) {
    int potentialE = std::abs(moon.pos.x) + std::abs(moon.pos.y) + std::abs(moon.pos.z);
    int kineticE = std::abs(moon.vel.x) + std::abs(moon.vel.y) + std::abs(moon.vel.z);
    return potentialE * kineticE;
}

void StepMoons(std::array<Moon, 4>& moons) {
    for (int i = 0; i < 4; i++) {
        for (int j = i+1; j < 4; j++) {
            Moon& a = moons[i];
            Moon& b = moons[j];
            Gravity(a.pos.x, b.pos.x, a.vel.x, b.vel.x);
            Gravity(a.pos.y, b.pos.y, a.vel.y, b.vel.y);
            Gravity(a.pos.z, b.pos.z, a.vel.z, b.vel.z);
        }
    }
    for (auto& m : moons) {
        m.pos = m.pos + m.vel;
    }
}

struct Args {
    bool part2 = false;
};

int main(int argc, char** argv) {
    clue::CommandLine<Args> cl;
    cl.Optional(&Args::part2, "part2");
    auto args = cl.ParseArgs(argc, argv);

    std::array<Moon, 4> moons = {{
        {Io, {3, -6, 6}, {0, 0, 0}},
        {Europa, {10, 7, -9}, {0, 0, 0}},
        {Ganymede, Vec3{-3, -7, 9}, Vec3{0, 0, 0}},
        {Callisto, {-8, 0, 4}, {0, 0, 0}}
    }};


    if (!args->part2) {
        for (int s = 0; s < 1000; s++) {
            StepMoons(moons);
        }
        int totalE = 0;
        for (const auto& m : moons) {
            totalE += Energy(m);
        }
        printf("Total Energy: %d\n", totalE);
    } else {
        float startX[4];
        float startY[4];
        float startZ[4];

        int64_t xSteps = -1;
        int64_t ySteps = -1;
        int64_t zSteps = -1;
        for (int i = 0; i < 4; i++) {
            startX[i] = moons[i].pos.x;
            startY[i] = moons[i].pos.y;
            startZ[i] = moons[i].pos.z;
        }
        int count = 0;
        while(true) {
            StepMoons(moons);
            count++;
    
            if (xSteps == -1 &&
                startX[0] == moons[0].pos.x && 0 == moons[0].vel.x &&
                startX[1] == moons[1].pos.x && 0 == moons[1].vel.x &&
                startX[2] == moons[2].pos.x && 0 == moons[2].vel.x &&
                startX[3] == moons[3].pos.x && 0 == moons[3].vel.x) {
                xSteps = count; 
            }
            if (ySteps == -1 &&
                startY[0] == moons[0].pos.y && 0 == moons[0].vel.y &&
                startY[1] == moons[1].pos.y && 0 == moons[1].vel.y &&
                startY[2] == moons[2].pos.y && 0 == moons[2].vel.y &&
                startY[3] == moons[3].pos.y && 0 == moons[3].vel.y) {
                ySteps = count; 
            }
            if (zSteps == -1 &&
                startZ[0] == moons[0].pos.z && 0 == moons[0].vel.z &&
                startZ[1] == moons[1].pos.z && 0 == moons[1].vel.z &&
                startZ[2] == moons[2].pos.z && 0 == moons[2].vel.z &&
                startZ[3] == moons[3].pos.z && 0 == moons[3].vel.z) {
                zSteps = count; 
            }
            if (xSteps != -1 && ySteps != -1 && zSteps != -1) {
                int64_t steps = std::lcm(std::lcm(xSteps, ySteps), zSteps);
                printf("%lld\n", steps);
                break;
            } 
        }
    }
}

