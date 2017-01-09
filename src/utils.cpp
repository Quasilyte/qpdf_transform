#include "utils.hpp"

#include <cstdlib>
#include <cerrno>

#include <sys/stat.h>

bool file_exists(const char* name) {
  struct stat file_info;
  return stat(name, &file_info) == 0;
}

long try_parse_int(const char* digits, ErrorCode failure_code) {
    char* tail;
    long result = strtol(digits, &tail, 10);
    long len = tail - digits;

    if (len == 0 || errno == ERANGE) {
        throw failure_code;
    }

    return result;
}

