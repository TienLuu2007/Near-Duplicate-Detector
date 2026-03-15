#include "function.h"

void shingle(std::vector<std::string> tokens, int k, std::unordered_set<std::string>& shingles)
{
    int n = tokens.size();
    std::string o = "";
    
    if (n <= k)
    {
        for (int i = 0; i < n; i++)
        {
            if (o != "") o += " ";
            o += tokens[i];
        }
        shingles.insert(o);
        return;
    }
    
    for (int i = 0; i <= n - k; i++)
    {
        o = "";
        for (int j = i; j < i + k; j++)
        {
            if (o != "") o += " ";
            o += tokens[j];
        }
        shingles.insert(o);
    }
}