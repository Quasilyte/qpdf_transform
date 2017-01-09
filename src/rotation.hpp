#pragma once

struct Rotation {
    Rotation();
    Rotation(const char* digits);

    int degree;
    bool absolute;
};
