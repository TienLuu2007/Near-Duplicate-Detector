#ifndef SHINGLER_H
#define SHINGLER_H

#include <string>
#include <set>

class Shingler 
{
public:
    // Shingle size of 4 to 9 for efficiency
    static std::set<std::string> get_shingles(const std::string& text, int k = 4);
};

#endif