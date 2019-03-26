#pragma once

#include <png.h>

#include <cstdio>
#include <filesystem>
#include <fstream>

#include "codecs/image.h"

class PNG {
public:
    PNG() = default;
    PNG(Image img);
    PNG(const PNG &) = delete;
    PNG &operator=(const PNG &) = delete;
    ~PNG();

    void load(std::filesystem::path path);
    Image slice(Vec pos, Vec size);

    Vec image_size;
    std::string text;

protected:
    class ReadHandle {
    public:
        ReadHandle();
        ~ReadHandle();

        png_structp png;
        png_infop info;
    };

    class WriteHandle {
    public:
        WriteHandle();
        ~WriteHandle();
    };

    uint8_t **rows;
};
