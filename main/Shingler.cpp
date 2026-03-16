#include "Shingler.h"

std::set<std::string> Shingler::get_shingles(const std::string& text, int k) 
{
    std::set<std::string> shingles;
    if (text.length() < k) return shingles;
    for (size_t i = 0; i <= text.length() - k; ++i) {
        shingles.insert(text.substr(i, k));
    }
    return shingles;
}