#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include <algorithm>
#include <cctype>

class Preprocessor {
public:
    static std::string clean(const std::string& text) {
        std::string cleaned;
        bool lastWasSpace = false;

        for (char c : text) {
            // 1. Lowercase everything
            if (std::isalnum(c)) {
                cleaned += std::tolower(c);
                lastWasSpace = false;
            } 
            // 2. Normalize spaces and strip punctuation
            else if (std::isspace(c)) {
                if (!lastWasSpace && !cleaned.empty()) {
                    cleaned += ' ';
                    lastWasSpace = true;
                }
            }
        }
        
        // Trim trailing space if necessary
        if (!cleaned.empty() && cleaned.back() == ' ') {
            cleaned.pop_back();
        }

        return cleaned;
    }
};

#endif