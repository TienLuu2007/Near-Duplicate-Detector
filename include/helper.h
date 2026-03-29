#ifndef _HELPER_H
#define _HELPER_H

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <map>

#include "preprocessor.h"
#include "shingler.h"
#include "MinHasher.h"
#include "LSH.h"
#include "json.hpp"

struct IndexedData 
{
    std::map<std::string, std::set<std::string>> shingles;
    std::map<std::string, std::vector<uint64_t>> signatures;
};

std::vector<uint64_t> getSignature(std::set<std::string> &shingles, MinHash &minhasher);

IndexedData index_original_documents(nlohmann::json &library, MinHash &minhasher, LSHIndex &lsh);

nlohmann::json load_from_dataset(const std::string &path);

std::string load_text_from_file(const std::string &file_path);

double getJaccard(const std::set<std::string> &shingles1, const std::set<std::string> &shingles2);

#endif