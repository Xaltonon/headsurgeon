#pragma once

#include <exception>
#include <string>

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
