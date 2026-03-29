#ifndef SHINGLER_H
#define SHINGLER_H

#include <string>
#include <set>

#include "config.hpp"

class Shingler 
{
    public:
        static std::set<std::string> get_shingles(const std::string& text, int k = Config::K_GRAM);
};

#endif