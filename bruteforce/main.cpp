/*RUN CODE
g++ -std=c++17 -O2 -o duplicate_detector main.cpp BruteForceDetector.cpp Preprocessor.cpp
./duplicate_detector
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

#include "BruteForceDetector.h"
#include "Preprocessor.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// Load dataset from JSON file
json load_dataset(const string& path) 
{
    ifstream file(path);
    if (!file.is_open()) 
    {
        cerr << "Failed to open dataset file: " << path << endl;
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

void write_result_to_file(const string& filename, 
                          const vector<tuple<string, string, double>>& results,
                          const json& dataset,
                          int total_comparisons,
                          double duration_seconds,
                          int k,
                          double threshold)
{
    ofstream out_file(filename);
    
    if (!out_file.is_open()) 
    {
        cerr << "Failed to open output file: " << filename << endl;
        return;
    }
    
    // Write header
    out_file << "============================================================" << endl;
    out_file << "           BRUTE-FORCE NEAR-DUPLICATE DETECTION            " << endl;
    out_file << "============================================================" << endl;
    out_file << endl;
    
    // Write parameters
    out_file << "=== Parameters ===" << endl;
    out_file << "Shingle size (k): " << k << endl;
    out_file << "Similarity threshold: " << threshold << endl;
    out_file << "Total comparisons: " << total_comparisons << endl;
    out_file << "Execution time: " << fixed << setprecision(3) << duration_seconds << " seconds" << endl;
    out_file << endl;
    
    // Write result
    out_file << "=== RESULT ===" << endl;
    
    int correct = 0;
    int total_expected = 0;
    
    for (const auto& [key, item] : dataset.items()) 
    {
        if (item.contains("variants")) 
        {
            for (const auto& variant : item["variants"]) 
            {
                string variant_id = variant["id"];
                total_expected++;
                
                // Find if this variant was matched correctly
                bool found = false;
                string matched_corpus = "";
                double matched_sim = 0.0;
                
                for (const auto& [query_id, corpus_id, similarity] : results) 
                {
                    if (query_id == variant_id) 
                    {
                        matched_corpus = corpus_id;
                        matched_sim = similarity;
                        if (corpus_id == key) 
                        {
                            found = true;
                        }
                        break;
                    }
                }
                
                if (found) 
                {
                    correct++;
                    out_file << "  [OK] " << variant_id << " -> " << key 
                             << " (sim: " << fixed << setprecision(4) << matched_sim << ")" << endl;
                } 
                else 
                {
                    if (!matched_corpus.empty()) 
                    {
                        out_file << "  [FAIL] " << variant_id << " -> " << matched_corpus 
                                 << " (expected: " << key << ", sim: " << matched_sim << ")" << endl;
                    } 
                    else 
                    {
                        out_file << "  [FAIL] " << variant_id << " -> NO MATCH" 
                                 /*<< " (expected: " << key << ")"*/ << endl;
                    }
                }
            }
        }
    }
    
    out_file << endl;
    out_file << "Accuracy: " << correct << "/" << total_expected 
             << " (" << fixed << setprecision(2) << (100.0 * correct / total_expected) << "%)" << endl;
    out_file << endl;
    
    // Write summary
    out_file << "=== Summary ===" << endl;
    out_file << "Total corpus documents: " << dataset.size() << endl;
    
    int total_variants = 0;
    for (const auto& [key, item] : dataset.items()) 
    {
        if (item.contains("variants")) 
        {
            total_variants += item["variants"].size();
        }
    }
    out_file << "Total query documents: " << total_variants << endl;
    out_file << "Matches found: " << results.size() << " / " << total_variants << endl;
    out_file << "Correct matches: " << correct << " / " << total_expected << endl;
    
    out_file.close();
    
    cout << "Results written to: " << filename << endl;
}

void run_bruteforce_on_dataset(const string& dataset_path, int k = 4, double threshold = 0.7) 
{
    cout << "Loading dataset from: " << dataset_path << endl;
    json dataset = load_dataset(dataset_path);
    
    if (dataset.empty()) 
    {
        cerr << "Dataset is empty or failed to load." << endl;
        return;
    }
    
    BruteForceDetector detector(k, threshold);
    
    // Step 1: Add all original documents as CORPUS documents
    cout << "\n=== Loading Corpus Documents (Reference Collection) ===" << endl;
    for (const auto& [key, item] : dataset.items()) 
    {
        string original_text = item["original"];
        string cleaned_text = Preprocessor::clean(original_text);
        detector.add_corpus_document(key, cleaned_text);
        cout << "  Added corpus document: " << key << endl;
    }
    
    // Step 2: Add all variant documents as QUERY documents
    cout << "\n=== Loading Query Documents (Documents to Check) ===" << endl;
    for (const auto& [key, item] : dataset.items()) 
    {
        if (item.contains("variants")) 
        {
            for (const auto& variant : item["variants"]) 
            {
                string variant_id = variant["id"];
                string variant_text = variant["content"];
                string cleaned_text = Preprocessor::clean(variant_text);
                detector.add_query_document(variant_id, cleaned_text);
                cout << "  Added query document: " << variant_id << " (expected to match " << key << ")" << endl;
            }
        }
    }
    
    // Print statistics to terminal
    detector.print_statistics();
    
    cout << "\n" << string(60, '=') << endl;
    
    // Step 3: Run brute-force detection
    auto start = chrono::high_resolution_clock::now();
    auto results = detector.find_best_matches();
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    double duration_seconds = duration.count() / 1000.0;
    
    cout << "\nTime taken: " << duration_seconds << " seconds" << endl;
    
    // Step 4: Print results to terminal
    cout << "\n=== Final Results ===" << endl;
    cout << left << setw(25) << "Query Document" 
         << setw(25) << "Matched Corpus Document" 
         << "Similarity" << endl;
    cout << string(65, '-') << endl;
    
    for (const auto& [query_id, corpus_id, similarity] : results) 
    {
        cout << left << setw(25) << query_id 
             << setw(25) << corpus_id 
             << fixed << setprecision(4) << similarity << endl;
    }
    
    // Step 5: Get total comparisons count from detector (approximate)
    int total_comparisons = results.size() * dataset.size();  // Rough estimate
    
    // Step 6: Write results to output file
    write_result_to_file("output.txt", results, dataset, total_comparisons, duration_seconds, k, threshold);
    
    // Step 7: Print verification to terminal
    cout << "\n=== Ground Truth Verification ===" << endl;
    int correct = 0;
    int total_expected = 0;
    
    for (const auto& [key, item] : dataset.items()) 
    {
        if (item.contains("variants")) 
        {
            for (const auto& variant : item["variants"]) 
            {
                string variant_id = variant["id"];
                total_expected++;
                
                bool found = false;
                string matched_corpus = "";
                double matched_sim = 0.0;
                
                for (const auto& [query_id, corpus_id, similarity] : results) 
                {
                    if (query_id == variant_id) 
                    {
                        matched_corpus = corpus_id;
                        matched_sim = similarity;
                        if (corpus_id == key) 
                        {
                            found = true;
                        }
                        break;
                    }
                }
                
                if (found) 
                {
                    correct++;
                    cout << "  ✓ " << variant_id << " correctly matched with " << key 
                         << " (sim: " << fixed << setprecision(4) << matched_sim << ")" << endl;
                } 
                else 
                {
                    if (!matched_corpus.empty()) 
                    {
                        cout << "  ✗ " << variant_id << " incorrectly matched with " << matched_corpus 
                             << " (expected: " << key << ", sim: " << matched_sim << ")" << endl;
                    } 
                    else 
                    {
                        cout << "  ✗ " << variant_id << " found no match (expected: " << key << ")" << endl;
                    }
                }
            }
        }
    }
    
    cout << "\nAccuracy: " << correct << "/" << total_expected 
         << " (" << fixed << setprecision(2) << (100.0 * correct / total_expected) << "%)" << endl;
}

int main(int argc, char* argv[]) 
{
    string dataset_path = "D:/Minh_Tri/hoctap/DAI HOC/nam 1/CTDL_GT/Project/dataset/text_variations.json";
    int k = 4;
    double threshold = 0.7;
    
    // Parse command line arguments
    if (argc > 1) dataset_path = argv[1];
    if (argc > 2) k = stoi(argv[2]);
    if (argc > 3) threshold = stod(argv[3]);
    
    run_bruteforce_on_dataset(dataset_path, k, threshold);
    cout << "|-|DONE|-|";
    return 0;
}