#include <iostream>
#include <string>
#include <vector>
#include <set>

// Include our three pipeline modules
#include "Shingler.h"
#include "MinHasher.h"
#include "LSH.h"
#include "Preprocessor.h"

using namespace std;

void index_document(const string& doc_id, const string& text, MinHash& hasher, LSHIndex& index) 
{
    
    string cleaned_text = Preprocessor::clean(text);
    
    set<string> shingles = Shingler::get_shingles(cleaned_text, 4);
    
    vector<uint64_t> signature = hasher.compute_signature(shingles);
    index.add_to_index(doc_id, signature);
    
    cout << "Indexed (cleaned): " << doc_id << endl;
}

int main() {
    MinHash minhasher(100); 
    LSHIndex lsh_index(20, 5);

    string doc1 = "The bus plunged into the ravine in Peru killing 23.";
    string doc2 = "Bus plunge into ravine in Peru killing 23";
    string doc3 = "The weather in Seattle is quite rainy today.";

    index_document("DOC_001_PERU_BUS", doc1, minhasher, lsh_index);
    index_document("DOC_002_SEATTLE_WX", doc3, minhasher, lsh_index);

    string cleaned_query = Preprocessor::clean(doc2);
    set<string> query_shingles = Shingler::get_shingles(cleaned_query, 4);
    vector<uint64_t> query_signature = minhasher.compute_signature(query_shingles);

    set<string> candidates = lsh_index.query_candidates(query_signature);

    // Display the results
    if (candidates.empty()) {
        cout << "No near-duplicates found." << endl;
    } else {
        cout << "Potential near-duplicates found:" << endl;
        for (const string& candidate_id : candidates) {
            cout << " -> " << candidate_id << endl;
        }
    }

    return 0;
}