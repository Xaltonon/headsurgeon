#pragma once

#include <Magick++.h>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <string>
#include <vector>

class DMI {
public:
    class State {
    public:
        State(std::string name);

        void load(Magick::Image dmi, unsigned width, unsigned height,
                  unsigned &index);
        void split(std::string format, std::filesystem::path path);

        std::string name;
        unsigned dirs = 1;
        unsigned frames = 1;
        std::vector<unsigned> delays;

    protected:
        static const char *dirname(unsigned d);

        void write_frames(unsigned dir, std::string format,
                          std::filesystem::path path);

        std::vector<std::vector<Magick::Image>> images;
    };

    void load(std::filesystem::path fname);
    void split(std::string format, std::filesystem::path path);

protected:
    void load_states(std::string data);

    std::string name;
    float version;
    unsigned width, height;
    std::vector<State> states;
};

class DMIError : public std::exception {
public:
    virtual std::string describe() = 0;
};

class ParseError : public DMIError {
public:
    ParseError(const char *reason);

    std::string describe() override;

protected:
    const char *reason;
};

class VersionError : public DMIError {
public:
    VersionError(float got, float expected);

    std::string describe() override;

protected:
    float got, expected;
};
