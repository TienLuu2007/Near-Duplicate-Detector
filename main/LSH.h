#ifndef LSHINDEX_H
#define LSHINDEX_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <cstdint>

class LSHIndex 
{
private:
    int num_bands;  
    int rows_per_band;
    
    // Band Index -> (Bucket Hash -> List of Document IDs)
    std::map<int, std::map<uint64_t, std::vector<std::string>>> index;

public:
    // b * r == signature size
    LSHIndex(int b = 20, int r = 5);

    // Adds a signature to the buckets
    void add_to_index(const std::string& doc_id, const std::vector<uint64_t>& signature);

    // Returns a list of potential duplicate IDs
    std::set<std::string> query_candidates(const std::vector<uint64_t>& signature);
};

#endif