
#include <string>
#include <iostream>
#include <cmath>

bool CouldBePassword(int number) {
    int digits[6];
    for (int i = 5; i >= 0; i--) {
        digits[i] = number % 10;
        number /= 10;
    }
    bool adjacentSame = false;
    bool neverDecrease = true;
    for (int i = 1; i < 6; i++) {
        adjacentSame  = adjacentSame  || (digits[i-1] == digits[i]);
        neverDecrease = neverDecrease && (digits[i-1] <= digits[i]);
    }
    return adjacentSame && neverDecrease;
}

bool NewRules(int number) {
    int digits[8] = {-1, 0, 0, 0, 0, 0, 0, -1};
    for (int i = 6; i >= 1; i--) {
        digits[i] = number % 10;
        number /= 10;
    }
    bool adjacentSame = false;
    bool neverDecrease = true;
    for (int i = 2; i < 7; i++) {
        adjacentSame  = adjacentSame
            || (digits[i-2] != digits[i-1] && digits[i-1] == digits[i] && digits[i] != digits[i+1]);
        neverDecrease = neverDecrease && (digits[i-1] <= digits[i]);
    }
    return adjacentSame && neverDecrease;
}

int main(int argc, char** argv) {
    int min = 234208;
    int max = 765869;
    int possibles = 0;
    int newPossibles = 0;
    for (int n = min; n <= max; n++) {
        if (CouldBePassword(n)) {
            possibles++;
        }
        if (NewRules(n)) {
            newPossibles++;
        }
    }
    std::cout << possibles << "\n";
    std::cout << newPossibles << "\n";
}

