#include "error_codes.hpp"
#include "utils.hpp"
#include "rotation.hpp"
#include "page_range.hpp"

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFWriter.hh>

typedef QPDFObjectHandle QpdfObject;

// g++ -O2 -std=c++14 -Iinclude -lqpdf -lpthread sed/page_range.cpp sed/rotation.cpp sed/utils.cpp sed/qpdf_rotate.cpp -o bin/qpdf

struct Arguments {
    const char* in_file_name;
    const char* out_file_name;
    char* pages = nullptr;
    const char* range = nullptr;
    Rotation degree;
};

void print_error(ErrorCode code);
Arguments parse_arguments(int argc, char* argv[]);
void rotate_pages(QPDF& in, QPDF& out, char* selector, Rotation);
void rotate_page_range(QPDF& in, QPDF& out, const char* range, Rotation);
void rotate_all_pages(QPDF& in, QPDF& out, Rotation);

int main(int argc, char* argv[]) {
    try {
        Arguments args = parse_arguments(argc, argv);

        QPDF in_pdf;
        in_pdf.processFile(args.in_file_name);
        QPDF out_pdf;
        out_pdf.emptyPDF();

        if (args.pages) {
            rotate_pages(in_pdf, out_pdf, args.pages, args.degree);
        } else if (args.range) {
            rotate_page_range(in_pdf, out_pdf, args.range, args.degree);
        } else {
            rotate_all_pages(in_pdf, out_pdf, args.degree);
        }

        QPDFWriter out_writer{out_pdf, args.out_file_name};
        out_writer.write();
    } catch (ErrorCode e) {
        print_error(e);
        return static_cast<int>(e);
    } catch (...) {
        print_error(ErrorCode::UnexpectedException);
        return static_cast<int>(ErrorCode::UnexpectedException);
    }

    return static_cast<int>(ErrorCode::Success);
}

// <program-name> + <in-pdf> + <degree> + <out-pdf>
const int minExpectedArgs = 4;

// If error message printing is not required, compile with -DNO_OUTPUT
#ifdef NO_OUTPUT
void usage() {}
void printError(ErrorCode) {}
#else
void usage() {
    puts(
        "usage: `qpdf-rotate <in-pdf> <degree> <out-pdf> [<page-selector>]`\n"
        "<in-pdf>: path to input pdf file\n"
        "<degree>: any 90 divisible angle; + or - forces relative rotation\n"
        "<out-pdf>: path for output pdf which will be created\n"
        "note: <in-pdf> must be distinct from <out-pdf>\n"
        "\n"
        "optional page selector arg:\n"
        "one of two possible formats:\n"
        "1) `--range=<low>-<high>` pages in range (use `z` for last page)\n"
        "2) `--pages=<p1>,...,<pn>` only specified pages (trailing comma is OK)\n"
        "note: (2) option needs sorted comma separated list\n"
        "\n"
        "example: `qpdf-rotate foo.pdf +90 bar.pdf --range=1-10\n"
        "example: `qpdf-rotate foo.pdf 0 bar.pdf --pages=1,2,3,7,10`\n"
        "example: `qpdf-rotate foo.pdf -90 bar.pdf --range=5-z"
    );
}

const char* error_message(ErrorCode code) {
    switch (code) {
    case ErrorCode::InvalidArgCount:
        return "<in-pdf> or <degree> or <out-pdf> arg is missing";
    case ErrorCode::InputFileNotExist:
        return "<in-pdf> file not found (not exists or can not be accessed)";
    case ErrorCode::InvalidDegreeArg:
        return "<degree> invalid value given";
    case ErrorCode::InvalidPageSelectorArg:
        return "<page-selector> invalid value given";
    case ErrorCode::InvalidRangeSelector:
        return "invalid range selector";
    case ErrorCode::InvalidPagesSelector:
        return "invalid pages selector";
    case ErrorCode::BadLowRangeBound:
        return "bad low range boundary";
    case ErrorCode::BadHighRangeBound:
        return "bad high range boundary";
    case ErrorCode::UnexpectedException:
        return "unexpected exception during execution";
    case ErrorCode::InternalRangeError:
        return "internal range error";

    default:
        return "";
    }
}

void print_error(ErrorCode code) {
    fprintf(stderr, "%s\n", error_message(code));
}
#endif

Arguments parse_arguments(int argc, char* argv[]) {
    if (argc < minExpectedArgs) {
        if (argc == 1) { // Launched without args
            usage();
        }

        throw ErrorCode::InvalidArgCount;
    }

    enum Argv: int {
        ProgramName,
        InputFile,
        Degree,
        OutputFile,
        PageSelector
    };

    Arguments args;

    args.in_file_name = argv[Argv::InputFile];
    args.out_file_name = argv[Argv::OutputFile];

    if (!file_exists(args.in_file_name)) throw ErrorCode::InputFileNotExist;

    args.degree = Rotation{argv[Argv::Degree]};

    if (argc > minExpectedArgs) { // Page selector given as an argument
        char* page_selector_arg = argv[Argv::PageSelector];

        if (has_substr(page_selector_arg, "--range=")) {
            args.range = fetch_range_selector(page_selector_arg);
        } else if (has_substr(page_selector_arg, "--pages=")) {
            args.pages = fetch_pages_selector(page_selector_arg);
        } else {
            throw ErrorCode::InvalidPageSelectorArg;
        }
    }

    return args;
}

void add_rotated_page(QPDF& pdf, QpdfObject& page, Rotation rotation) {
    page.replaceKey("/Rotate", calculate_degree(page, rotation));
    pdf.addPage(page, false);
}

void add_rotated_pages(QPDF& pdf, PageRange pages, Rotation rotation) {
    for (auto page : pages) {
        add_rotated_page(pdf, page, rotation);
    }
}

void add_pages(QPDF& pdf, PageRange pages) {
    for (auto page : pages) {
        pdf.addPage(page, false);
    }
}

void rotate_pages(QPDF& in, QPDF& out, char* selector, Rotation rotation) {
    static const int unparsed = -1;

    auto pages = in.getAllPages();
    auto digits = strtok(selector, ",");
    int n = unparsed;

    for (int page_n = 0; page_n < pages.size(); ++page_n) {
        if (digits && n == unparsed) {
            n = try_parse_int(digits, ErrorCode::BadPageNumber) - 1;
        }

        if (n == page_n) {
            digits = strtok(nullptr, ",");
            n = unparsed;
            add_rotated_page(out, pages[page_n], rotation);
        } else {
            out.addPage(pages[page_n], false);
        }
    }
}

void rotate_page_range(QPDF& in, QPDF& out, const char* range, Rotation rotation) {
    auto pages = in.getAllPages();

    int from_page = try_parse_int(range, ErrorCode::BadLowRangeBound);
    int to_page;

    if (range[(strlen(range)-1)] == 'z') {
        to_page = pages.size();
    } else {
        to_page = try_parse_int(strchr(range, '-') + 1, ErrorCode::BadHighRangeBound);
    }

    if (from_page > 1) {
        add_pages(out, PageRange{1, from_page - 1, pages});
    }
    add_rotated_pages(out, PageRange{from_page, to_page, pages}, rotation);
    if (to_page < pages.size()) {
        add_pages(out, PageRange(to_page + 1, pages));
    }
}

void rotate_all_pages(QPDF& in, QPDF& out, Rotation rotation) {
    auto pages = in.getAllPages();
    add_rotated_pages(out, PageRange{1, pages}, rotation);
}
