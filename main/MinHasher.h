#ifndef MINHHASHER_H
#define MINHHASHER_H

#include <vector>
#include <string>
#include <set>
#include <cstdint>

struct HashParams
{
    uint64_t a;
    uint64_t b;
};

class MinHash
{
    private:
        int signature_size;
        uint64_t prime; // A large prime number
        std::vector<HashParams> hash_funcs;
    public:
        MinHash(int signature_size = 100, uint64_t prime = 4294967291ull);
        std::vector<uint64_t> compute_signature(const std::set<std::string>& shingles);
};

#endif