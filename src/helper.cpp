#include "helper.h"

std::vector<uint64_t> getSignature(std::set<std::string> &shingles, MinHash &minhasher) 
{
    std::vector<uint64_t> signature = minhasher.compute_signature(shingles);
    return signature;
}

IndexedData index_original_documents(nlohmann::json &dataset, MinHash &minhasher, LSHIndex &lsh_index)
{
    IndexedData data;
    for(const auto &obj : dataset)
    {
        std::string doc_id = obj["Source_id"].get<std::string>();
        std::string relative_path = obj["Source_file"].get<std::string>();
        std::string full_path = relative_path;
        std::string extracted_text = load_text_from_file(full_path);
        std::set<std::string> shingles = Shingler::get_shingles(extracted_text, 4);
        std::vector<uint64_t> signature = getSignature(shingles, minhasher);

        std::set<std::string> shingles = Shingler::get_shingles(extracted_text, 4);
        std::vector<uint64_t> signature = getSignature(shingles, minhasher);

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