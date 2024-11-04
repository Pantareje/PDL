#pragma once

#include <string>

enum class TaskType {
    None, Tokens, Symbols, Parse
};

struct ApplicationAttributes {
    TaskType taskType;
    std::string inputFileName;
    std::string outputFileName;
};
