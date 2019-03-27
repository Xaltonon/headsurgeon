#pragma once

#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include "codecs/image.h"
#include "codecs/png.h"
#include "errors.h"

class DMI {
public:
    class State {
    public:
        State(std::string name);

        void load(PNG &png, unsigned width, unsigned height, unsigned &index);
        void split(std::filesystem::path path);

        std::string name;
        unsigned dirs = 1;
        unsigned frames = 1;
        unsigned loop = 0;
        std::vector<float> delays;

    protected:
        static const char *dirname(unsigned d);

        void write_frames(unsigned dir,
                          std::filesystem::path path);

        std::vector<std::vector<Image>> images;
    };

    void load(std::filesystem::path fname);
    void split(std::filesystem::path path);

protected:
    void load_states(std::string data);

    std::string name;
    float version;
    unsigned width, height;
    std::vector<State> states;
};
