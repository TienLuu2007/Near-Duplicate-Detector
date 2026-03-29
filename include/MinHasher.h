#ifndef MINHHASHER_H
#define MINHHASHER_H

#include <vector>
#include <string>
#include <set>
#include <cstdint>

#include "config.hpp"

struct HashParams
{
    uint64_t a;
    uint64_t b;
};

class MinHash
{
    private:
        int signature_size;
        uint64_t prime;
        std::vector<HashParams> hash_funcs;
    public:
        // Initializes random hash functions for MinHash
        MinHash(int sig_size = Config::SIGNATURE_SIZE, uint64_t p = 4294967311ULL);

        // Computes the MinHash signature for a given set of shingles
        std::vector<uint64_t> compute_signature(const std::set<std::string> &shingles);

        // Calculates the similarity between two MinHash signatures
        double calculate_similarity(const std::vector<uint64_t> &sig1, const std::vector<uint64_t> &sig2);
};

#endif