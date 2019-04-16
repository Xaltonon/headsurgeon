#pragma once

#include <filesystem>
#include <vector>
#include <webp/demux.h>

#include "libheadsurgeon/codecs/image.hpp"

class WebP {
public:
    WebP() = default;
    WebP(std::vector<Image> frames, std::vector<float> delays, unsigned loops);

    void save(std::filesystem::path path);
    void load(std::filesystem::path path);

    std::vector<Image> frames;
    std::vector<float> delays;
    unsigned loops;

protected:
    struct DecoderHandle {
        DecoderHandle(WebPData *data, WebPAnimDecoderOptions *opts);
        ~DecoderHandle();

        WebPAnimDecoder *d;
    };
};
