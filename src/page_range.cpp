#include "page_range.hpp"

#include "error_codes.hpp"

PageRange::PageRange(int from, std::vector<QPDFObjectHandle>& pages):
from_page{from}, to_page{static_cast<int>(pages.size())}, pages{pages} {
    check_invariants();
}

PageRange::PageRange(int from, int to, std::vector<QPDFObjectHandle>& pages):
from_page{from}, to_page{to}, pages{pages} {
    check_invariants();
}

void PageRange::check_invariants() {
    if (from_page < 1 || to_page < 1) {
        throw UnexpectedException;
    }
}
