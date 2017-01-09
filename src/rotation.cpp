#include "rotation.hpp"

#include "error_codes.hpp"
#include "utils.hpp"

#include <cctype>

Rotation::Rotation() {}

Rotation::Rotation(const char* digits) {
    absolute = isdigit(*digits);
    degree = try_parse_int(digits, InvalidRotationArg);

    if (degree % 90 != 0) {
        throw InvalidRotationArg;
    }
}
