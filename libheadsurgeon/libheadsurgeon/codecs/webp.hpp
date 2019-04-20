#pragma once

#include <vector>
#include <webp/demux.h>

#include "libheadsurgeon/filesystem.hpp"
#include "libheadsurgeon/codecs/image.hpp"

class WebP {
public:
    WebP() = default;
    WebP(std::vector<Image> frames, std::vector<float> delays, unsigned loops);

    void save(fs::path path);
    void load(fs::path path);

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
