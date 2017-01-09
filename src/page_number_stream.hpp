#pragma once

//! @brief Простая обёртка для итерирования по строке, содержащей список страниц
class PageNumberStream {
public:
    PageNumberStream(const char* pages);

    bool has_next() const;
    void next();
    int current() const;

private:
    const char* pos;
    const char* pos_max;
};
