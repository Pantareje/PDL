#pragma once

#include <string>

enum class TaskType {
    None, Tokens, Symbols, Parse
};

struct ApplicationAttributes {
    TaskType taskType;
    bool useSemantic;
    std::string inputFileName;
    std::string outputFileName;
};
