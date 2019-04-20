#pragma once

#include <png.h>

#include <cstdio>
#include <fstream>

#include "libheadsurgeon/filesystem.hpp"
#include "libheadsurgeon/codecs/image.hpp"

class PNG {
public:
    PNG() = default;
    PNG(Vec size);
    PNG(const PNG &) = delete;
    PNG &operator=(const PNG &) = delete;
    ~PNG();

    void load(fs::path path);
    Image slice(Vec pos, Vec size);

    void save(fs::path path);
    void insert(Vec pos, Image img);

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

        png_structp png;
        png_infop info;
    };

    uint8_t **rows = nullptr;
};
