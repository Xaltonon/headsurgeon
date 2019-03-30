#include "codecs/png.h"
#include "codecs/file.h"
#include "errors.h"

#include <iostream>
#include <algorithm>
#include <functional>

PNG::ReadHandle::ReadHandle() {
    png = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, nullptr,
        [](png_structp, const char *) { throw ParseError("internal libpng read error"); },
        nullptr);
    if (!png)
        throw ParseError("couldn't create libpng read struct");

    info = png_create_info_struct(png);
    if (!info)
        throw ParseError("couldn't create PNG info struct");
}

PNG::ReadHandle::~ReadHandle() {
    png_destroy_read_struct(&png, &info, nullptr);
}

PNG::WriteHandle::WriteHandle() {
    png = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, nullptr,
        [](png_structp, const char *) { throw ParseError("internal libpng write error"); },
        nullptr);
    if (!png)
        throw ParseError("couldn't create libpng write struct");

    info = png_create_info_struct(png);
    if (!info)
        throw ParseError("couldn't create PNG info struct");
}

PNG::WriteHandle::~WriteHandle() {
    png_destroy_write_struct(&png, &info);
}

PNG::~PNG() {
    if (!rows)
        return;
    for (unsigned i = 0; i < image_size.y; i++)
        delete[] rows[i];
    delete[] rows;
}

void PNG::load(std::filesystem::path path) {
    FileHandle file(path.c_str(), "rb");

    uint8_t sig[8];
    fread(sig, 1, 8, file.file);
    if (!png_check_sig(sig, 8))
        throw ParseError("not a PNG file");

    ReadHandle handle;

    png_init_io(handle.png, file.file);
    png_set_sig_bytes(handle.png, 8);
    png_read_info(handle.png, handle.info);

    png_set_palette_to_rgb(handle.png);
    png_set_tRNS_to_alpha(handle.png);

    image_size = {png_get_image_width(handle.png, handle.info),
                  png_get_image_height(handle.png, handle.info)};

    rows = new uint8_t *[image_size.y];
    for (unsigned i = 0; i < image_size.y; i++)
        rows[i] = new uint8_t[image_size.x * 4];

    png_set_rows(handle.png, handle.info, rows);
    png_read_image(handle.png, rows);

    png_textp t;
    int num;
    png_get_text(handle.png, handle.info, &t, &num);

    if (num < 1)
        throw ParseError("incorrect PNG text blocks");

    text = std::string(t->text);
}

Image PNG::slice(Vec pos, Vec size) {
    Image ret;
    ret.size = size;
    ret.pixels.resize(size.x * size.y * 4);
    for (unsigned i = 0; i < size.y; i++) {
        std::copy_n(rows[i + pos.y] + pos.x * 4, size.x * 4,
                    ret.pixels.begin() + i * size.x * 4);
    }
    return ret;
}
