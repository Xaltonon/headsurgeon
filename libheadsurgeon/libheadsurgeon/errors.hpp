#pragma once

#include <exception>
#include <string>

class DMIError : public std::exception {
public:
    virtual std::string describe() const = 0;
};

class ReasonError : public DMIError {
public:
    ReasonError(const char *reason);

    std::string describe() const override;

protected:
    const char *reason;
};

class VersionError : public DMIError {
public:
    VersionError(float got, float expected);

    std::string describe() const override;

protected:
    float got, expected;
};
