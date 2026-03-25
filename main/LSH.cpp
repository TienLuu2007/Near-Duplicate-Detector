#include "LSH.h"

LSHIndex::LSHIndex(int b, int r) : num_bands(b), rows_per_band(r) {}

void LSHIndex::add_to_index(const std::string& doc_id, const std::vector<uint64_t>& signature) 
{
    for (int b = 0; b < num_bands; ++b) 
    {
        // Get the slice of the signature for this band
        uint64_t bucket_hash = 0;
        for (int r = 0; r < rows_per_band; ++r) 
        {
            uint64_t val = signature[b * rows_per_band + r];
            bucket_hash ^= val + 0x9e3779b9 + (bucket_hash << 6) + (bucket_hash >> 2);
        }

        // Put the document ID into the bucket for this specific band
        index[b][bucket_hash].push_back(doc_id);
    }
}

std::set<std::string> LSHIndex::query_candidates(const std::vector<uint64_t>& signature) 
{
    std::set<std::string> candidates;

    for (int b = 0; b < num_bands; ++b) 
    {
        // Calculate the bucket hash for the query signature in this band
        uint64_t bucket_hash = 0;
        for (int r = 0; r < rows_per_band; ++r) 
        {
            uint64_t val = signature[b * rows_per_band + r];
            bucket_hash ^= val + 0x9e3779b9 + (bucket_hash << 6) + (bucket_hash >> 2);
        }

        // If this bucket exists in this band, everyone in it is a candidate
        if (index[b].count(bucket_hash)) 
        {
            for (const std::string& id : index[b][bucket_hash]) 
            {
                candidates.insert(id);
            }
        }
    }
    return candidates;
}