#include "function.h"

double Jaccard(std::vector<uint32_t> sig0, std::vector<uint32_t> sig)
{
    int l = sig0.size();
    int t = 0;
    
    for (int i = 0; i < sseed; i++)
    {
        if (sig0[i] == sig[i]) 
            t++;
    }
    
    return (t * (1.0) / sig0.size()) * 100;
}