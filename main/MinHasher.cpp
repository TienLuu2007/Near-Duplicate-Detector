#include "MinHasher.h"
#include <functional>
#include <climits>
#include <random>

MinHash::MinHash(int sig_size, uint64_t p) : signature_size(sig_size), prime(p) 
{
    // Use a fixed seed for reproducibility
    std::mt19937 rng(1337); 
    std::uniform_int_distribution<uint64_t> dist(1, prime - 1);

    for (int i = 0; i < signature_size; i++) 
    {
        hash_funcs.push_back({dist(rng), dist(rng)});
    }
}

std::vector<uint64_t> MinHash::compute_signature(const std::set<std::string>& shingles) 
{
    std::vector<uint64_t> signature(signature_size, ULLONG_MAX);
    std::hash<std::string> string_hasher;

    for (const auto& shingle : shingles) 
    {
        // Compress the string into a 64-bit integer
        uint64_t shingle_hash = string_hasher(shingle);
        
        // Apply Universal Hashing to find the minimums
        for (int i = 0; i < signature_size; i++) 
        {
            uint64_t hash_val = (hash_funcs[i].a * shingle_hash + hash_funcs[i].b) % prime;
            
            if (hash_val < signature[i]) 
            {
                signature[i] = hash_val;
            }
        }
    }
    return signature;
}