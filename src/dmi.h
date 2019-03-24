#pragma once

#include <cstdio>
#include <exception>
#include <string>
#include <vector>

#include <png.h>

class DMI {
public:
    class State {
    public:
        State(std::string name, unsigned width, unsigned height);

        void load(uint8_t **rows, unsigned sw, unsigned &index);

        std::string name;
        unsigned dirs = 1;
        unsigned frames = 1;
        std::vector<unsigned> delays;

        unsigned width, height;

    protected:
        std::vector<std::vector<std::vector<uint8_t>>> pixels;
    };

    void load(FILE *fp);

protected:
    void load_states(png_structp png, png_infop info);

    float version;
    unsigned width, height;
    std::vector<State> states;

    unsigned sheetw, sheeth;
};

class DMIError : public std::exception {
public:
    virtual std::string describe() = 0;
};

class ParseError : public DMIError {
public:
    ParseError(const char *reason);
    ParseError(FILE *fp, const char *reason);

    std::string describe() override;

protected:
    unsigned pos;
    const char *reason;
};

class VersionError : public DMIError {
public:
    VersionError(float got, float expected);

    std::string describe() override;

protected:
    float got, expected;
};
