#pragma once

#include <cstdio>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "libheadsurgeon/codecs/image.hpp"
#include "libheadsurgeon/codecs/png.hpp"
#include "libheadsurgeon/errors.hpp"
#include "libheadsurgeon/export.hpp"

class DMI {
public:
    class State {
    public:
        State(std::string name);

        void load(PNG &png, unsigned width, unsigned height, unsigned &index);
        void split(std::filesystem::path path);

        void save(PNG &png, unsigned &index);
        void join(std::filesystem::path path);

        std::string name;
        unsigned dirs = 1;
        unsigned frames = 1;
        unsigned loop = 0;
        std::vector<float> delays;

        Vec size() const;

    protected:
        static const char *dirname(unsigned d);
        static int dirnum(const std::string name);

        void write_frames(unsigned dir, std::filesystem::path path);
        void reduplicate();

        std::vector<std::vector<Image>> images;
    };

    LIBHEADSURGEON_SYMEXPORT void load(std::filesystem::path fname);
    LIBHEADSURGEON_SYMEXPORT void
    split(std::filesystem::path path,
          std::function<void(int total, int i, std::string name)> callback);

    LIBHEADSURGEON_SYMEXPORT void save(std::filesystem::path fname);
    LIBHEADSURGEON_SYMEXPORT void
    join(std::filesystem::path path,
         std::function<void(int total, int i, std::string name)> callback);

    std::string name;
    float version = 4.0;
    unsigned width, height;
    std::vector<State> states;

protected:
    void load_states(std::string data);
    std::string gen_states();
};
