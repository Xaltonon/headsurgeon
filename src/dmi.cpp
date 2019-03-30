#include "dmi.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <sstream>

#include "codecs/png.h"
#include "codecs/webp.h"
#include "errors.h"

namespace fs = std::filesystem;

void DMI::load(fs::path fname) {
    name = fname.stem();
    PNG png;
    png.load(fname);

    std::string data = png.text;
    load_states(data);

    unsigned index = 0;
    for (auto &s : states) {
        s.load(png, width, height, index);
    }
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
                dmi_str >> cur.loop;
            } else if (property == "rewind") {
            } else {
                throw ParseError{"unknown DMI property encountered"};
            }

            getline(dmi_str, '\n');
        }
    }
}

DMI::State::State(std::string name) : name(name) {}

void DMI::State::load(PNG &png, unsigned width, unsigned height,
                      unsigned &index) {
    images.resize(dirs);
    for (unsigned i = 0; i < dirs; i++)
        images[i].resize(frames);

    for (unsigned f = 0; f < frames; f++) {
        for (unsigned d = 0; d < dirs; d++) {
            unsigned x = (index * width) % png.image_size.x;
            unsigned y = (index / (png.image_size.x / width)) * height;
            images[d][f] = png.slice({x, y}, {width, height});
            index++;
        }
    }
}

void DMI::split(fs::path path,
                std::function<void(int, int, std::string)> callback) {
    fs::create_directory(path);
    int i = 1;
    int total = states.size();
    for (auto &s : states) {
        callback(total, i, s.name);
        s.split(path / s.name);
        i++;
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

void DMI::State::split(fs::path path) {
    if (dirs == 1) {
        write_frames(0, path);
        return;
    }

    fs::create_directory(path);
    for (unsigned d = 0; d < dirs; d++) {
        write_frames(d, path / dirname(d));
    }
}

void DMI::State::write_frames(unsigned int dir,
                              std::filesystem::path path) {
    WebP webp{images[dir], delays, loop};
    webp.save(path.replace_extension("webp"));
}
