#include "rotation_arg.hpp"

#include "error_codes.hpp"

#include <cstring>

RotationArg::RotationArg(const char* arg):
rotation(Rotation(arg)) {
    pages = strchr(arg, '=');
    if (pages == NULL) { // '=' не найден
        throw InvalidRotationArg;
    } else {
        pages += 1; // Пропуск '='
    }
}
