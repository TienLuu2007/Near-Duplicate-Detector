#include "function.h"

uint32_t hashfunction(const std::string& shingle, uint32_t seed) {
    const uint32_t FNV_PRIME = 16777619;
    uint32_t hash = 2166136261 ^ seed;
    
    for (char c : shingle) {
        hash ^= static_cast<uint32_t>(c);
        hash *= FNV_PRIME;
    }
    
    return hash;
}

std::vector<uint32_t> Minhash(std::unordered_set<std::string> shingles)
{
    std::vector<uint32_t> signatures(sseed, UINT32_MAX);
    
    for (const std::string &x : shingles)
    {
        for (int seed = 0; seed < sseed; seed++)
        {
            uint32_t hash_val = hashfunction(x, seed + 1);
            if (signatures[seed] > hash_val) 
                signatures[seed] = hash_val;
        }
    }
    
    return signatures;
}