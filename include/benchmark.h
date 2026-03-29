#ifndef _BENCHMARK_H
#define _BENCHMARK_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>

#include "helper.h"
#include "json.hpp"
#include "config.hpp"

struct metrics
{
    // --- Basic Detection Counts ---
    int ground_truth_match = 0; // ID matches parent exactly
    int collateral_match = 0;   // ID is different, but J >= threshold
    int false_positives = 0;    // J < threshold
    int false_negatives = 0;    // Parent not found OR (Parent found but J < threshold)
    
    // --- Efficiency & Probability ---
    int total_queries = 0;
    int total_candidates_considered = 0; // All hits returned by LSH before Jaccard check
    int total_docs_in_library = 0;       // For Selectivity calculation
    double total_query_time_ms = 0.0;
    
    // --- MinHash Accuracy (MAE) ---
    double total_jaccard_error = 0.0;
    int total_comparisons = 0;           // Number of Jaccard pairs calculated
    
    // --- Ranking ---
    double mrr_sum = 0.0;                // For Mean Reciprocal Rank

    // --- Metrics Ready ---

    double precision_at_threshold() const 
    {
        int true_positives = ground_truth_match + collateral_match;
        int total_positives = true_positives + false_positives;
        return (total_positives == 0) ? 0.0 : (double)true_positives / total_positives;
    }

    double recovery_rate() const 
    {
        return (total_queries == 0) ? 0.0 : (double)ground_truth_match / total_queries;
    }

    double mae() const 
    {
        return (total_comparisons == 0) ? 0.0 : total_jaccard_error / total_comparisons;
    }

    double avg_query_time() const 
    {
        return (total_queries == 0) ? 0.0 : total_query_time_ms / total_queries;
    }

    double selectivity() const 
    {
        if (total_queries == 0 || total_docs_in_library == 0) return 0.0;
        return (double)total_candidates_considered / (total_queries * total_docs_in_library);
    }

    double mrr() const 
    {
        return (total_queries == 0) ? 0.0 : mrr_sum / total_queries;
    }
};

metrics runBenchmark(MinHash &minhasher, LSHIndex &lsh_index,
const nlohmann::json &original_library, const nlohmann::json &target_suite, const std::string &project_root);

#endif