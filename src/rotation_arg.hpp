#pragma once

#include "rotation.hpp"

// Rotation, который требуется применить к pages страницам.
struct RotationArg {
    RotationArg(const char* arg);

    Rotation rotation;
    const char* pages;
};
