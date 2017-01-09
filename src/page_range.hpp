#pragma once

#include <qpdf/QPDF.hh>
#include <vector>

//! @brief Обёртка вокруг vector<QPDFObjectHandle> диапазона
struct PageRange {
    PageRange(int from, std::vector<QPDFObjectHandle>& pages);

    PageRange(int from, int to, std::vector<QPDFObjectHandle>& pages);

    void check_invariants();

    QPDFObjectHandle* begin() const noexcept { return pages.data() + from_page - 1; }
    QPDFObjectHandle* end() const noexcept { return pages.data() + to_page; }

private:
    std::vector<QPDFObjectHandle>& pages;
    int to_page;
    int from_page;
};
