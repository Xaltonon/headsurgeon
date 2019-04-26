#include <sstream>

#include "libheadsurgeon/errors.hpp"

ReasonError::ReasonError(const char *reason) : reason(reason) {}

std::string ReasonError::describe() const { return reason; }

VersionError::VersionError(float got, float expected)
    : got(got), expected(expected) {}

std::string VersionError::describe() const {
    std::stringstream s;
    s << "Expected DMI version " << expected << ", got " << got;
    return s.str();
}
