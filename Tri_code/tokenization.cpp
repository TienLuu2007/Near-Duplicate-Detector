#include "function.h"

std::string normalize(std::string& s)
{
    std::string ans = "";
    for (char x : s)
        if (std::isalnum(x) or std::isspace(x))
            ans += tolower(x);
    return ans;
}

void tokenize(std::string s, std::vector<std::string>& tokens)
{
    std::stringstream ss(s);
    std::string o;
    
    while (ss >> o)
    {
        tokens.push_back(o);
    }
}