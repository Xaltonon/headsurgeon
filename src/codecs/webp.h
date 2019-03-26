#pragma once

#include <filesystem>
#include <vector>

#include "codecs/image.h"

class WebP {
public:
    WebP() = default;
    WebP(std::vector<Image> frames, std::vector<float> delays, unsigned loops);

    void save(std::filesystem::path path);

protected:
    std::vector<Image> frames;
    std::vector<float> delays;
    unsigned loops;
};
