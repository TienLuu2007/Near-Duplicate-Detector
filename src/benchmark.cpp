#include "benchmark.h"

Tracker::Tracker(int total) : query_count(0), total_queries(total) 
{
    std::cout << "\n--- Starting Benchmark Loop ---" << std::endl;
}

metrics LocalitySensitiveHashing(MinHash &minhasher, LSHIndex &lsh_index,
const nlohmann::json &original_library, const nlohmann::json &target_suite)
{
    IndexedData document_features = 
        index_original_documents(const_cast<nlohmann::json&>(original_library), minhasher, lsh_index);

    metrics res;
    Tracker tracker(res.total_queries);
    timer query_timer;
    query_timer.start();

    res.total_docs_in_library = original_library.size();
    res.total_queries = target_suite.size();

    for (const auto &obj : target_suite)
    {
        // --- Safe guard ---
        bool has_id = obj.contains("Target_id");
        bool has_truth = obj.contains("Ground_truth");
        bool has_file = obj.contains("Target_file");
        
        if (!has_id || !has_truth || !has_file) 
        {
            std::cerr << "\nCRITICAL ERROR in target_suite_master.json!" << std::endl;
            std::cerr << "Problem found at entry index: " << tracker.get_query_count() << std::endl;
            
            if (!has_id)    std::cerr << " -> Missing key: 'Target_id'" << std::endl;
            if (!has_truth) std::cerr << " -> Missing key: 'Ground_truth'" << std::endl;
            if (!has_file)  std::cerr << " -> Missing key: 'Target_file'" << std::endl;
            
            std::cerr << "Full Object Content causing the crash:\n" << obj.dump(4) << std::endl;
            
            continue; 
        }
        // --------------------

        tracker.tick(obj["Target_id"].get<std::string>());
        
        std::string target_id = obj["Target_id"].get<std::string>();
        std::string ground_truth_id = obj["Ground_truth"].get<std::string>();
        std::string relative_path = obj["Target_file"].get<std::string>();
        std::string full_path = "./" + relative_path;
        std::string extracted_text = load_text_from_file(full_path);

        if (extracted_text.empty()) 
        {
            std::cerr << "Warning: Could not read target file: " << full_path << std::endl;
            continue;
        }

        std::set<std::string> target_shingles = Shingler::get_shingles(extracted_text, 4);
        std::vector<uint64_t> query_signature = minhasher.compute_signature(target_shingles);

        std::set<std::string> candidates = lsh_index.query_candidates(query_signature);

        res.total_candidates_considered += candidates.size();

        bool parent_found_in_lsh = false;
        std::vector<std::pair<std::string, double>> ranked_candidates;

        for (const auto& candidate_id : candidates)
        {
            // Safeguard: Check if candidate_id actually exists in indexed features
            if (document_features.shingles.find(candidate_id) == document_features.shingles.end()) 
            {
                continue; 
            }

            double real_jaccard = getJaccard(target_shingles, document_features.shingles[candidate_id]);
            double estimated_jaccard = minhasher.calculate_similarity(query_signature, document_features.signatures[candidate_id]);
            
            res.total_jaccard_error += std::abs(real_jaccard - estimated_jaccard);
            res.total_comparisons++;

            if (real_jaccard >= Config::THRESHOLD) 
            {
                ranked_candidates.push_back({candidate_id, real_jaccard});
                
                if (candidate_id == ground_truth_id) 
                {
                    res.ground_truth_match++;
                    parent_found_in_lsh = true;
                } 
                else 
                {
                    res.collateral_match++;
                }
            } 
            else 
            {
                res.false_positives++;
            }
        }

        if (!parent_found_in_lsh) 
        {
            res.false_negatives++;
        }

        std::sort(ranked_candidates.begin(), ranked_candidates.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });

        for (size_t i = 0; i < ranked_candidates.size(); ++i) 
        {
            if (ranked_candidates[i].first == ground_truth_id) 
            {
                res.mrr_sum += 1.0 / (i + 1);
                break;
            }
        }
    }
    
    res.total_query_time_ms += query_timer.elapsed_ms();

    return res;
}

metrics BruteForce(const nlohmann::json &original_library, const nlohmann::json &target_suite)
{
    IndexedData document_features;

    for(const auto &obj : original_library)
    {
        std::string doc_id = obj["Source_id"].get<std::string>();
        std::string relative_path = obj["Source_file"].get<std::string>();
        std::string full_path = "./" + relative_path;
        std::string extracted_text = load_text_from_file(full_path);
        std::set<std::string> shingles = Shingler::get_shingles(extracted_text, 4);
        
        document_features.shingles[doc_id] = shingles;
    }

    metrics res;
    timer run_timer;
    run_timer.start();
    Tracker tracker(target_suite.size());
    
    for (const auto &obj : target_suite)
    {
        // --- Safe guard ---
        bool has_id = obj.contains("Target_id");
        bool has_truth = obj.contains("Ground_truth");
        bool has_file = obj.contains("Target_file");
        
        if (!has_id || !has_truth || !has_file) 
        {
            std::cerr << "\nCRITICAL ERROR in target_suite_master.json!" << std::endl;
            std::cerr << "Problem found at entry index: " << tracker.get_query_count() << std::endl;
            
            if (!has_id)    std::cerr << " -> Missing key: 'Target_id'" << std::endl;
            if (!has_truth) std::cerr << " -> Missing key: 'Ground_truth'" << std::endl;
            if (!has_file)  std::cerr << " -> Missing key: 'Target_file'" << std::endl;
            
            std::cerr << "Full Object Content causing the crash:\n" << obj.dump(4) << std::endl;
            
            continue; 
        }
        // --------------------

        tracker.tick(obj["Target_id"].get<std::string>());

        std::string target_id = obj["Target_id"].get<std::string>();
        std::string ground_truth_id = obj["Ground_truth"].get<std::string>();
        std::string relative_path = obj["Target_file"].get<std::string>();
        std::string full_path = "./" + relative_path;
        std::string extracted_text = load_text_from_file(full_path);

        if (extracted_text.empty()) 
        {
            std::cerr << "Warning: Could not read target file: " << full_path << std::endl;
            continue;
        }
        
        std::set<std::string> target_shingles = Shingler::get_shingles(extracted_text, 4);
        
        std::string best_match_id = "None";
        double best_jaccard = 0.0;
        std::vector<std::pair<std::string, double>> ranked_candidates;

        for(const auto &[doc_id, doc_shingles] : document_features.shingles)
        {
            double jaccard = getJaccard(target_shingles, doc_shingles);
            if(jaccard >= Config::THRESHOLD)
            {
                res.collateral_match++;
                ranked_candidates.push_back({doc_id, jaccard});

                if(jaccard > best_jaccard)
                {
                    best_jaccard = jaccard;
                    best_match_id = doc_id;
                }
            }

        }

        bool found_ground_truth = false;
        for (const auto& candidate : ranked_candidates)
        {
            if (candidate.first == ground_truth_id) 
            {
                res.ground_truth_match++;
                found_ground_truth = true;
                break;
            }
        }

        if(found_ground_truth)
        {
            std::sort(ranked_candidates.begin(), ranked_candidates.end(), 
                      [](const auto& a, const auto& b) { return a.second > b.second; });
            for (size_t i = 0; i < ranked_candidates.size(); ++i)
            {
                if (ranked_candidates[i].first == ground_truth_id) 
                {
                    res.mrr_sum += 1.0 / (i + 1);
                    break;
                }
            }
        }
    }

    res.total_query_time_ms = run_timer.elapsed_ms();

    return res;
}

void timer::start() 
{
    start_time = std::chrono::high_resolution_clock::now();
}

double timer::elapsed_ms() const 
{
    auto end_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end_time - start_time).count();
}