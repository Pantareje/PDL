#pragma once

#include <string>
#include <map>

struct Symbol {
    std::string lex;
    std::map<std::string, std::string> attributes;
};
