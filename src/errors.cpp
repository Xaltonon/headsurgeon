#include "errors.h"

#include <sstream>

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
