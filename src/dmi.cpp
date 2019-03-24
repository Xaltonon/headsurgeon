#include "dmi.h"

#include <Magick++.h>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

void DMI::load(fs::path fname) {
    name = fname.stem();
    Magick::Image dmi{fname};
    std::string data = dmi.attribute("Description");
    load_states(data);

    unsigned index = 0;
    for (auto &s : states) {
        s.load(dmi, width, height, index);
    }

    split("gif", "out");
}

static std::string getline(std::istream &input, char delim) {
    std::string s;
    std::getline(input, s, delim);
    return s;
}

void DMI::load_states(std::string data) {
    std::istringstream dmi_str{data};
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
        states.emplace_back(name);
        DMI::State &cur = states[states.size() - 1];

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
}

DMI::State::State(std::string name) : name(name) {}

void DMI::State::load(Magick::Image dmi, unsigned width, unsigned height,
                      unsigned &index) {
    images.resize(dirs);
    for (unsigned f = 0; f < frames; f++) {
        for (unsigned d = 0; d < dirs; d++) {
            auto &frame = images[d].emplace_back(dmi);
            unsigned x = (index * width) % dmi.size().width();
            unsigned y = (index / (dmi.size().width() / width)) * height;
            frame.crop({width, height, x, y});
            frame.repage();
            frame.gifDisposeMethod(Magick::BackgroundDispose);
            index++;
        }
    }
}

void DMI::split(std::string format, fs::path path) {
    fs::create_directory(path);
    for (auto &s : states) {
        std::cout << s.name << "\n";
        s.split(format, path / s.name);
    }
}

const char *DMI::State::dirname(unsigned d) {
    switch (d) {
    case 0:
        return "down";
    case 1:
        return "up";
    case 2:
        return "right";
    case 3:
        return "left";
    case 4:
        return "downright";
    case 5:
        return "downleft";
    case 6:
        return "upright";
    case 7:
        return "upleft";
    default:
        throw ParseError("unknown direction");
    }
}

void DMI::State::split(std::string format, fs::path path) {
    if (dirs == 1) {
        write_frames(0, format, path);
        return;
    }

    fs::create_directory(path);
    for (unsigned d = 0; d < dirs; d++) {
        write_frames(d, format, path / dirname(d));
    }
}

void DMI::State::write_frames(unsigned int dir, std::string format,
                              std::filesystem::path path) {
    Magick::writeImages(images[dir].begin(), images[dir].end(),
                        path.string() + "." + format);
}

ParseError::ParseError(const char *reason) : reason(reason) {}

std::string ParseError::describe() {
    std::stringstream s;
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
