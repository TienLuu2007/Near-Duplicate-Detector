#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include <algorithm>
#include <cctype>

class Preprocessor {
public:
    static std::string clean(const std::string& text);
};

#endif