#include "Preprocessor.h"

std::string Preprocessor::clean(const std::string& text) {
    std::string cleaned;
    cleaned.reserve(text.size());
    
    for (char c : text) {
        if (std::isalnum(c)) {
            cleaned += std::tolower(c);
        } else if (std::isspace(c)) {
            cleaned += ' ';
        }
    }
    
    // Remove extra spaces
    auto end = std::unique(cleaned.begin(), cleaned.end(), [](char a, char b) {
        return std::isspace(a) && std::isspace(b);
    });
    cleaned.erase(end, cleaned.end());
    
    // Trim leading and trailing spaces
    size_t start = cleaned.find_first_not_of(' ');
    size_t end_pos = cleaned.find_last_not_of(' ');
    
    if (start == std::string::npos) {
        return ""; // String is all spaces
    }
    
    return cleaned.substr(start, end_pos - start + 1);
}