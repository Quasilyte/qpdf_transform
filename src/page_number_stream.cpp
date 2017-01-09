#include "page_number_stream.hpp"

#include "utils.hpp"
#include "error_codes.hpp"

#include <cstring>

PageNumberStream::PageNumberStream(const char* pages):
pos(pages), pos_max(pages + strlen(pages)) {}

bool PageNumberStream::has_next() const {
    return pos < pos_max;
}

void PageNumberStream::next() {
    while (pos < pos_max && isdigit(*pos)) {
        pos += 1;
    }

    // Лишняя запятая на конце допускается
    if (*pos != ',' && pos != pos_max) {
        throw BadPageNumber;
    } else {
        pos += 1; // Пропуск ','
    }
}

int PageNumberStream::current() const {
    long n = try_parse_int(pos, BadPageNumber);
    if (n < 1) { // Номер страницы не может быть отрицательным или нулевым
        throw BadPageNumber;
    }

    return n;
}
