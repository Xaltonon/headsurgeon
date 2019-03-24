#include "dmi.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <Magick++.h>

void DMI::load(FILE *fp) {
    uint8_t header[8];
    if (fread(header, 1, 8, fp) != 8)
        throw ParseError{fp, "no PNG header"};

    if (png_sig_cmp(header, 0, 8))
        throw ParseError{fp, "invalid PNG header"};

    png_structp png;
    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr,
                                 nullptr);
    if (!png)
        throw ParseError{fp, "couldn't create PNG read struct"};

    png_infop info;
    info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        throw ParseError{fp, "couldn't create PNG info struct"};
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        throw ParseError{fp, "internal libpng error"};
    }

    png_init_io(png, fp);
    png_set_sig_bytes(png, 8);

    png_read_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);

    if (png_get_channels(png, info) != 4)
        throw ParseError{fp, "expected 4 channel PNG"};

    sheetw = png_get_image_width(png, info);
    sheeth = png_get_image_height(png, info);

    load_states(png, info);

    png_destroy_read_struct(&png, &info, nullptr);
}

static std::string getline(std::istream &input, char delim) {
    std::string s;
    std::getline(input, s, delim);
    return s;
}

void DMI::load_states(png_structp png, png_infop info) {
    int num;
    png_textp text;
    png_get_text(png, info, &text, &num);

    if (num != 1)
        throw ParseError{"too many PNG text blocks"};

    std::istringstream dmi_str{text[0].text};
    if (getline(dmi_str, '\n') != "# BEGIN DMI")
        throw ParseError{"no DMI metadata"};

    getline(dmi_str, '=');
    dmi_str >> version;
    getline(dmi_str, '=');
    dmi_str >> width;
    getline(dmi_str, '=');
    dmi_str >> height;
    getline(dmi_str, '\n');

    if (dmi_str.fail())
        throw ParseError{"bad version/size header"};
    if (version != 4.0)
        throw VersionError(version, 4.0);

    while (getline(dmi_str, '"') == "state = ") {
        std::string name = getline(dmi_str, '"');
        if (name == "")
            name = "default";
        states.emplace_back(name, width, height);
        DMI::State &cur = states[states.size() - 1];
        std::cout << "state " << name << "\n";

        while (isspace(dmi_str.peek())) {
            while (isspace(dmi_str.peek()))
                dmi_str.get();

            std::string property = getline(dmi_str, ' ');
            getline(dmi_str, '=');

            if (property == "dirs") {
                dmi_str >> cur.dirs;
            } else if (property == "frames") {
                dmi_str >> cur.frames;
            } else if (property == "delay") {
                do {
                    dmi_str >> cur.delays.emplace_back();
                } while (dmi_str.peek() == ',' && dmi_str.get());
            } else if (property == "loop") {
            } else {
                throw ParseError{"unknown DMI property encountered"};
            }

            getline(dmi_str, '\n');
        }
    }

    uint8_t **rows;
    rows = png_get_rows(png, info);
    unsigned index = 0;
    for (auto &s : states)
        s.load(rows, sheetw, index);
}

DMI::State::State(std::string name, unsigned width, unsigned height)
    : name(name), width(width), height(height) {}

void DMI::State::load(uint8_t **rows, unsigned int sw, unsigned &index) {
    for (unsigned i = 0; i < dirs; i++) {
        pixels.emplace_back();
        for (unsigned j = 0; j < frames; j++) {
            pixels[i].emplace_back();
            pixels[i][j].resize(width * height * 4);
            unsigned x = (width * index) % sw;
            unsigned y = (width * index) / sw;
            for (unsigned k = 0; k < height; k++)
                std::copy_n(rows[y] + x, width,
                            pixels[i][j].begin() + k * width);
            index++;
        }
    }
}

ParseError::ParseError(const char *reason) : pos(0), reason(reason) {}

ParseError::ParseError(FILE *fp, const char *reason)
    : pos(ftell(fp)), reason(reason) {}

std::string ParseError::describe() {
    std::stringstream s;
    if (pos)
        s << "Error parsing DMI (" << reason << ") at position " << pos;
    else
        s << "Error parsing DMI (" << reason << ")";
    return s.str();
}

VersionError::VersionError(float got, float expected)
    : got(got), expected(expected) {}

std::string VersionError::describe() {
    std::stringstream s;
    s << "Expected DMI version " << expected << ", got " << got;
    return s.str();
}
