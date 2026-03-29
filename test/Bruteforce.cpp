#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>

#include "Shingler.h"
#include "helper.h"
#include "config.hpp"
#include "json.hpp"
#include "benchmark.h"

using json = nlohmann::json;

void report(const metrics res)
{
    std::ofstream report_file("bruteforce_benchmark.txt", std::ios::app);
    if (!report_file.is_open())
    {
        std::cerr << "Failed to open benchmark report file." << std::endl;
        return;
    }
    report_file << "\n--- Benchmark Results ---" << std::endl;
    report_file << "------------------------\n" << std::endl;
    report_file << "__Config settings: " << std::endl;
    report_file << "Threshold: " << Config::THRESHOLD << std::endl;
    report_file << "------------------------\n" << std::endl;
    report_file << "Total Queries Time (ms): " << res.total_query_time_ms << std::endl;
    report_file << "Average Query Time (ms): " << res.avg_query_time() << std::endl;
    report_file << "Recovery Rate: " << res.recovery_rate() << std::endl;
    report_file << "Mean Reciprocal Rank (MRR): " << res.mrr() << std::endl;
    report_file << "--- End of Results ---\n" << std::endl;
}

int main() {
    Config::setThreshold(0.7);
    Config::setKGram(4);

    std::string project_root = ".";
    std::string original_library_path = project_root + "/data/meta/original_lib_master.json";
    std::string target_suite_path = project_root + "/data/meta/target_suite_master.json";

    json original_library = load_from_dataset(original_library_path);
    json target_suite = load_from_dataset(target_suite_path);

    if (original_library.empty() || target_suite.empty()) 
    {
        std::cerr << "One or more datasets are empty or failed to load." << std::endl;
        return 1;
    }

    report(BruteForce(original_library, target_suite));

    return 0;
}