#include "benchmark.h"

metrics runBenchmark(MinHash &minhasher, LSHIndex &lsh_index,
const nlohmann::json &original_library, const nlohmann::json &target_suite, const std::string &project_root)
{
    IndexedData document_features = 
        index_original_documents(const_cast<nlohmann::json&>(original_library), minhasher, lsh_index);

    metrics res;
    res.total_docs_in_library = original_library.size();
    res.total_queries = target_suite.size();

    for (const auto& obj : target_suite)
    {
        std::string target_id = obj["Target_id"].get<std::string>();
        std::string ground_truth_id = obj["Ground truth"].get<std::string>();
        std::string relative_path = obj["Target_file"].get<std::string>();
        std::string full_path = project_root + relative_path;
        
        std::string extracted_text = load_text_from_file(full_path);
        
        auto start = std::chrono::high_resolution_clock::now();

        std::set<std::string> target_shingles = Shingler::get_shingles(extracted_text, 4);
        std::vector<uint64_t> query_signature = minhasher.compute_signature(target_shingles);

        std::set<std::string> candidates = lsh_index.query_candidates(query_signature);
        
        auto end = std::chrono::high_resolution_clock::now();
        res.total_query_time_ms += std::chrono::duration<double, std::milli>(end - start).count();

        res.total_candidates_considered += candidates.size();

        bool parent_found_in_lsh = false;
        std::vector<std::pair<std::string, double>> ranked_candidates;

        for (const auto& candidate_id : candidates)
        {
            double real_jaccard = getJaccard(target_shingles, document_features.shingles[candidate_id]);
            
            // Calculate MinHash Estimated Jaccard (for MAE)
            double estimated_jaccard = minhasher.calculate_similarity(query_signature, document_features.signatures[candidate_id]);
            
            res.total_jaccard_error += std::abs(real_jaccard - estimated_jaccard);
            res.total_comparisons++;

            if (real_jaccard >= Config::THRESHOLD) 
            {
                ranked_candidates.push_back({candidate_id, real_jaccard});
                
                if (candidate_id == ground_truth_id) {
                    res.ground_truth_match++;
                    parent_found_in_lsh = true;
                } else {
                    res.collateral_match++;
                }
            } 
            else 
            {
                res.false_positives++;
            }
        }

        // Handle false negatives
        if (!parent_found_in_lsh) {
            res.false_negatives++;
        }

        // Mean Reciprocal Rank
        std::sort(ranked_candidates.begin(), ranked_candidates.end(), 
                 [](const auto& a, const auto& b) { return a.second > b.second; });

        for (int i = 0; i < ranked_candidates.size(); ++i) {
            if (ranked_candidates[i].first == ground_truth_id) {
                res.mrr_sum += 1.0 / (i + 1);
                break;
            }
        }

    }

    return res;
}