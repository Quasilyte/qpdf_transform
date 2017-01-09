#pragma once

//! @brief Поворот страницы. Может быть абсолютным или относительным
struct Rotation {
    Rotation();
    Rotation(const char* digits);

    int degree;
    bool absolute; //!< Если false, то поворот относительный
};
