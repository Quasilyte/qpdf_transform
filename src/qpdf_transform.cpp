#include "error_codes.hpp"
#include "rotation.hpp"
#include "utils.hpp"
#include "page_number_stream.hpp"
#include "rotation_arg.hpp"

#include <map>
#include <vector>
#include <exception>
#include <cstring>

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFWriter.hh>

/*
Эта версия является портом под C++98 (с C++11).
Изменения:
    1) nullptr -> NULL
    2) удаление noexcept спецификаторов
    3) scoped enum (enum class) -> обычный enum
    4) extended initializer lists -> старый синтаксис инициализаторов (x{} -> x())
    5) вывод типов с auto -> явное описание типов
    6) unordered_map -> map
    7) for-range loops -> обычный for цикл с итераторами
    8) Constructor() = default -> ручное описание конструкторов по умолчанию
*/

using std::map;
using std::vector;

typedef QPDFObjectHandle QpdfObject;

//! @brief Аргументы командной строки после парсинга
struct Arguments {
    Arguments(): page_numbers(NULL) {}     

    const char* in_file_name;
    const char* out_file_name;
 
    char* page_numbers;
    vector<RotationArg> rotations;
};

static void usage();
static void print_error_message(const char*);
static void print_error(ErrorCode);
static Arguments parse_arguments(int argc, char* argv[]);
static void apply_transformations(QPDF& in, QPDF& out, char* page_numbers, vector<RotationArg>&);

int main(int argc, char* argv[]) {
    try {
        Arguments args = parse_arguments(argc, argv);

        QPDF in_pdf;
        in_pdf.processFile(args.in_file_name);
        QPDF out_pdf;
        out_pdf.emptyPDF();

        apply_transformations(in_pdf, out_pdf, args.page_numbers, args.rotations);
  
        QPDFWriter out_writer(out_pdf, args.out_file_name);
        out_writer.write();
    } catch (ErrorCode e) {
        print_error(e);
        return static_cast<int>(e);
    } catch (std::exception& e) {
        print_error_message(e.what());
        return static_cast<int>(UnexpectedException);       
    } catch (...) {
        print_error(UnexpectedException);
        return static_cast<int>(UnexpectedException);
    }

    return static_cast<int>(Success);
}

// <program-name> + <in-pdf> + <out-pdf> + <pages>
const int minExpectedArgs = 4;

// Разбор аргументов командной строки, который формирует объект Arguments.
static Arguments parse_arguments(int argc, char* argv[]) {
    if (argc < minExpectedArgs) {
        if (argc == 1) {
            usage();
        }

        throw InvalidArgCount;
    }

    enum Argv {
        ProgramName,
        InputFile,
        OutputFile,
        PageNumbers,
        Rotation
    };

    Arguments args;

    args.in_file_name = argv[InputFile];
    args.out_file_name = argv[OutputFile];
    args.page_numbers = argv[PageNumbers];

    if (!file_exists(args.in_file_name)) {
        throw InputFileNotExist;
    }
    if (strcmp(args.in_file_name, args.out_file_name) == 0) {
        throw InputAndOutputOverlap;
    }

    // Оставшиеся аргументы - это задаваемые развороты для страниц
    int rotationsCount = argc - Rotation;
    args.rotations.reserve(rotationsCount);
    for (int n = Rotation; n < argc; ++n) {
        args.rotations.push_back(RotationArg(argv[n]));
    }

    return args;
}

// Создание отображения [номер страницы]=>[разворот].
static map<int, Rotation> parse_rotations(vector<RotationArg>& rotations) {
    map<int, Rotation> result;

    for (int i = 0; i < (int)rotations.size(); ++i) {
        RotationArg rotation_arg = rotations[i];
        for (PageNumberStream numbers(rotation_arg.pages); numbers.has_next(); numbers.next()) {
            result[numbers.current()] = rotation_arg.rotation;
        }
    }

    return result;
}

/*!
 * @brief Вычислить вращение @p rotation для страницы @page с учётом абсолютных разворотов
 * @param page Страница PDF, для которой производится расчёт
 * @param rotation Переданный параметр вращения
 * @return Вычесленный Rotation приведённый в формат объекта QPDF
 */
static QpdfObject calculate_degree(QpdfObject& page, Rotation rotation) {
    int degree = rotation.degree;

    if (!rotation.absolute && page.hasKey("/Rotate")) {
        int old_degree = page.getKey("/Rotate").getNumericValue();
        degree += old_degree;
    }

    return QpdfObject::newInteger(degree);
}

static void apply_transformations(QPDF& in,
                                  QPDF& out,
                                  char* page_numbers,
                                  vector<RotationArg>& rotations) {
    map<int, Rotation> rotation_mapping = parse_rotations(rotations);
    vector<QpdfObject> pages = in.getAllPages();

    for (PageNumberStream numbers(page_numbers); numbers.has_next(); numbers.next()) {
        int number = numbers.current();

        if (number > (int)pages.size()) {
            throw BadPageNumber;
        }

        QpdfObject page = pages[number - 1]; // Страницы начинаются с 1, а индексы с 0 => отсюда -1
        map<int, Rotation>::iterator rotation = rotation_mapping.find(number);

        if (rotation != rotation_mapping.end()) {
            page.replaceKey("/Rotate", calculate_degree(page, rotation->second));
        }

        out.addPage(page, false);
    }
}

// Если скомпилировано с -DNO_OUTPUT, печать в консоль будет отключена
#ifdef NO_OUTPUT
static void usage() {}
static void print_error_message(const char*);
static void printError(ErrorCode) {}
#else
void usage() {
    puts(
        "usage: `qpdf-transform <in-pdf> <out-pdf> <page-numbers> [<rotations>...]`\n"
        "<in-pdf>: path to input pdf file\n"
        "<out-pdf>: path for output pdf which will be created\n"
        "<page-numbers>: comma separated list of page numbers (trailing comma is OK)\n"
        "<rotations>: relative or absolute angle followed by a page numbers\n"
        "note: <in-pdf> must be distinct from <out-pdf>\n"
        "note: numbers in <page-numbers> must be unique (no duplicates allowed)\n"
        "\n"
        "examples\n"
        "1) copy only 1,2 and 4 pages from a.pdf and save as b.pdf:\n"
        "qpdf-transform a.pdf b.pdf 1,2,4\n"
        "\n"
        "2) like 1 example, but also rotates 1 and 2 pages 90 clockwise degrees:\n"
        "qpdf-transform a.pdf b.pdf 1,2,4 +90=1,2\n"
        "\n"
        "3) different rotations:\n"
        "qpdf-transform a.pdf b.pdf 1,2,4 +180=1 -90=2 +90=4\n"
        "\n"
        "4) set fixed rotation:\n"
        "qpdf-transform a.pdf b.pdf 1,2,4 90=1,2\n"
        "\n"
        "5) set default rotation:\n"
        "qpdf-transform a.pdf b.pdf 1,2,4 0=1,2,4\n"
        "\n"
        "6) shuffle pages in reverse order:\n"
        "qpdf-transform a.pdf b.pdf 3,2,1\n"
    );
}

static const char* error_message(ErrorCode code) {
    switch (code) {
    case InvalidArgCount:
        return "<in-pdf> or <out-pdf> or <pages> arg is missing";
    case InputFileNotExist:
        return "<in-pdf> file not found (not exists or can not be accessed)";
    case InputAndOutputOverlap:
        return "<in-pdf> should not be the same as <out-pdf>";
    case InvalidRotationArg:
        return "invalid rotation value given";
    case UnexpectedException:
        return "unexpected exception during execution";
    case BadPageNumber:
        return "bad page number (negative or higher than max page number)";

    default:
        return "";
    }
}

static void print_error_message(const char* msg) {
    fprintf(stderr, "error: %s\n", msg);
}

static void print_error(ErrorCode code) {
    print_error_message(error_message(code));
}
#endif
