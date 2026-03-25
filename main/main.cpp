#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>

#include "Shingler.h"
#include "MinHasher.h"
#include "LSH.h"
#include "Preprocessor.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;


void index_document(const string& doc_id, const string& text, MinHash& hasher, LSHIndex& index) 
{
    string cleaned_text = Preprocessor::clean(text);
    set<string> shingles = Shingler::get_shingles(cleaned_text, 4);
    vector<uint64_t> signature = hasher.compute_signature(shingles);
    index.add_to_index(doc_id, signature);
    
    // cout << "Indexed (cleaned): " << doc_id << endl;
}

void query(MinHash &minhasher, LSHIndex &lsh_index, string &query_text, string &query_id)
{
    fstream log_file("query_log.txt", ios::app);
    if (!log_file.is_open()) {
        cerr << "Failed to open log file." << endl;
        return;
    }
    string cleaned_query = Preprocessor::clean(query_text);
    set<string> query_shingles = Shingler::get_shingles(cleaned_query, 4);
    vector<uint64_t> query_signature = minhasher.compute_signature(query_shingles);
    set<string> candidates = lsh_index.query_candidates(query_signature);

    // Display the results
    if (candidates.empty()) 
    {
        log_file << "No near-duplicates found for " << query_id << endl;
    } 
    else 
    {
        log_file << "Potential near-duplicates found for " << query_id << ":" << endl;
        for (const string& candidate_id : candidates) 
        {
            log_file << " -> " << candidate_id << endl;
        }
    }
    log_file.close();
}

json load_from_dataset(const string& path)
{

    ifstream file(path);
    if(!file.is_open())
    {
        cerr << "Failed to open dataset file." << endl;
        return json();
    }

    try 
    {
        json dataset;
        file >> dataset;
        return dataset;
    } 
    catch (json::parse_error& e) 
    {
        cerr << "JSON Parse Error: " << e.what() << endl;
        return json::object();
    }
}

void benchmark(MinHash &minhasher, LSHIndex &lsh_index, const json &dataset)
{
    for(const auto& [key, item] : dataset.items())
    {
        string original_text = item["original"];
        string original_id = key;
        index_document(original_id, original_text, minhasher, lsh_index);
    }
    for(const auto& [key, item] : dataset.items())
    {
        for(const auto &variant : item["variants"])
        {
            string variant_id = variant["id"];
            string variant_text = variant["content"];
            query(minhasher, lsh_index, variant_text, variant_id);
            // cout << "Indexed variant: " << variant_id << endl;
        }
    }
}

int main() {
    MinHash minhasher(100); 
    LSHIndex lsh_index(20, 5);

    string path = "../../Near-Dup dataset/IELTS/writing task 2/variant_samples.json";
    json dataset = load_from_dataset(path);
    if (dataset.empty()) 
    {
        cerr << "Dataset is empty or failed to load." << endl;
        return 1;
    }

    benchmark(minhasher, lsh_index, dataset);
    return 0;
}