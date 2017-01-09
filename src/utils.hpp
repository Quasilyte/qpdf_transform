#pragma once

#include "error_codes.hpp"
#include "rotation.hpp"

#include <qpdf/QPDF.hh>

//! @brief Существует ли файл с именем @p name
bool file_exists(const char* name);

/*!
 * @brief Получить числовое представление переданной строки из чисел
 * @param digits Строка, содержащая цифры числа
 * @param failure_code Объект, который будет выброшен в качестве исключения при ошибке
 * \return
 */
long try_parse_int(const char* digits, ErrorCode failure_code);
