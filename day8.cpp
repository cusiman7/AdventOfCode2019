
#include "clue.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

using Layer = std::vector<int>;

struct Image {
    int width;
    int height;
    int size;
    std::vector<Layer> layers;
};

constexpr int BLACK = 0;
constexpr int WHITE = 1;
constexpr int TRANSPARENT = 2;

Image ReadImage(std::istream& istream, int width, int height) {
    Image image;
    image.width = width;
    image.height = height;
    image.size = width * height;
    while (istream.good()) {
        std::string imageStr;
        istream >> imageStr;
        int n = 0;
        for (auto& c : imageStr) {
            if (n == 0) image.layers.emplace_back(image.size);
            image.layers.back()[n] = c - '0';
            n = ++n % image.size;
        }
    }
    return image;
}

Image DecodeImageLayers(const Image& image) {
    Image finalImage;
    finalImage.width = image.width;
    finalImage.height = image.height;
    finalImage.size = image.size;
    finalImage.layers.emplace_back(image.size, TRANSPARENT);
    for (const auto& layer : image.layers) {
        int n = 0;
        for (const int& pix : layer) {
            if (finalImage.layers.back()[n] == TRANSPARENT) {
                finalImage.layers.back()[n] = pix;
            }
            n++;
        }
    }
    return finalImage;
}

void RenderImage(const Image& image, bool invert) {
    int n = 0;
    const char* black = (invert) ? " " : u8"█";
    const char* white = (invert) ? u8"█" : " ";
    for (const auto& pix : image.layers.back()) {
        switch (pix) {
            case BLACK:
                std::cout << black;
                break;
            case WHITE:
                std::cout << white;
                break;
            case TRANSPARENT:
                std::cout << " ";
                break;
        }
        if (n == image.width - 1) {
            std::cout << "\n";
        }
        n = ++n % image.width;
    }
}

struct Args {
    std::string file = "day8.txt";
    std::array<int, 2> size = {25, 6};
    std::string test = "";
    bool part2 = false;
    bool invert = false;
};

int main(int argc, char** argv) {
    clue::CommandLine<Args> cl;
    cl.Optional(&Args::test, "test");
    cl.Optional(&Args::size, "size", "Image width and height");
    cl.Optional(&Args::part2, "part2");
    cl.Optional(&Args::invert, "invert");
    cl.Positional(&Args::file, "file");
    auto args = cl.ParseArgs(argc, argv);

    Image image;
    if (!args->test.empty()) {
        std::istringstream in(args->test);
        image = ReadImage(in, args->size[0], args->size[1]);
    } else if (!args->file.empty()) {
        std::ifstream file(args->file);
        image = ReadImage(file, args->size[0], args->size[1]);
    }

    int minZeros = std::numeric_limits<int>::max();
    int onesTimesTwos = -1;
    for (const auto& layer : image.layers) {
        int zeros = 0;
        int ones = 0;
        int twos = 0;
        for (const auto& pix : layer) {
            zeros += pix == 0 ? 1 : 0;
            ones  += pix == 1 ? 1 : 0;
            twos  += pix == 2 ? 1 : 0;
        }
        if (zeros < minZeros) {
            minZeros = zeros;
            onesTimesTwos = ones * twos;
        }
    }
    std::cout << onesTimesTwos << "\n";

    if (args->part2) {
        Image decodedImage = DecodeImageLayers(image);
        RenderImage(decodedImage, args->invert);
    }
}

