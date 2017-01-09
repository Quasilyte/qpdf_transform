#pragma once

// Расшифровки есть в функции error_message
enum ErrorCode {
    Success, // <- Не является ошибкой
    InvalidArgCount,
    InputFileNotExist,
    InputAndOutputOverlap,
    InvalidRotationArg,
    BadPageNumber,
    UnexpectedException,
};
