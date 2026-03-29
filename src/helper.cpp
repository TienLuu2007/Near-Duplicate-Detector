#include "helper.h"

std::vector<uint64_t> getSignature(std::set<std::string> &shingles, MinHash &minhasher) 
{
    std::vector<uint64_t> signature = minhasher.compute_signature(shingles);
    return signature;
}

IndexedData index_original_documents(nlohmann::json &dataset, MinHash &minhasher, LSHIndex &lsh_index) 
{
    IndexedData data;
    int index = 0;

    for (const auto &obj : dataset) 
    {
        index++;

        if (!obj.is_object()) 
        {
            std::cerr << "\nCRITICAL: Entry at index " << index << " is not a JSON object!" << std::endl;
            std::cerr << "Actual value: " << obj.dump() << std::endl;
            continue;
        }

        bool has_id = obj.contains("Source_id");
        bool has_file = obj.contains("Source_file");

        if (!has_id || !has_file) 
        {
            std::cerr << "\nCRITICAL: Missing keys at index " << index << std::endl;
            if (!has_id) std::cerr << " -> Missing 'Source_id'" << std::endl;
            if (!has_file) std::cerr << " -> Missing 'Source_file'" << std::endl;
            std::cerr << "Full Object Content: " << obj.dump(4) << std::endl;
            continue; 
        }

        std::string doc_id = obj["Source_id"].get<std::string>();
        std::string relative_path = obj["Source_file"].get<std::string>();
        
        std::string full_path = "./" + relative_path; 

        std::cout << "Processing [" << index << "]: " << doc_id << "..." << std::endl;

        std::string extracted_text = load_text_from_file(full_path);
        if (extracted_text.empty()) 
        {
             std::cerr << "Warning: Empty or missing file at " << full_path << std::endl;
             continue;
        }

        std::set<std::string> shingles = Shingler::get_shingles(extracted_text, Config::K_GRAM);
        std::vector<uint64_t> signature = minhasher.compute_signature(shingles);

        lsh_index.add_to_index(doc_id, signature);
        data.shingles[doc_id] = shingles;
        data.signatures[doc_id] = signature;
    }
    return data;
}

nlohmann::json load_from_dataset(const std::string &path)
{
    std::ifstream file(path);
    if(!file.is_open()) 
    {
        std::cerr << "Failed to open dataset file." << std::endl;
        return nlohmann::json();
    }
    try 
    {
        nlohmann::json dataset;
        file >> dataset;
        return dataset;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return nlohmann::json();
    }
}

std::string load_text_from_file(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open text file: " << path << std::endl;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

double getJaccard(const std::set<std::string> &shingles1, const std::set<std::string> &shingles2)
{
    size_t intersection_size = 0;
    for (const auto& shingle : shingles1) 
    {
        if (shingles2.count(shingle)) 
        {
            intersection_size++;
        }
    }

    size_t union_size = shingles1.size() + shingles2.size() - intersection_size;
    return (union_size == 0) ? 0.0 : static_cast<double>(intersection_size) / union_size;
}