# Near-Duplicate-Detector
A C++ implementation of MinHash and LSH for text similarity detection.

The purpose of this project is to demonstrate the strength of a method used in near-duplicate detection in comparison to the basic inefficient brute force approach. Locality Sensitive Hashing is the core optimization that makes this project scalable. There are many Locality Sensitive Hashing (LSH) algorithms available for solving this problem. Among them Minhash is a really popular and solid LSH algorithm that uses probabilistic method to generate similar fingerprints for similar objects.

# Problem definition
Given a large collection of text documents and a query document, we want to identify all documents in the collection that are most similar to query (near-duplicate). These are documents that share the same underlying text structure and wording but may have minor modifications, such as adding or removing a few words/lines or minor reordering of phrases.

This is different from exact duplicate detection, which only finds identical copies. Near-duplicate detection is more challenging because it must handle these small variations while still recognizing that the texts are essentially the same in terms of structure and wording.

To define the scope of our work, we impose a few constraints on the input. The documents are plain text files - no images, no tables, no complex formatting. The language is English, and the texts are assumed to be well-formed with standard characters.

For each query document, our system outputs all source documents in the collection whose similarity with the query exceeds a predefined threshold. This threshold can be adjusted depending on the experiment conduction.

# Pipeline
The detection engine follows a structured White-Box pipeline to transform raw text into searchable signatures:
* Shingling: Documents are decomposed into $k$-gram character shingles to capture local structure.
* MinHashing: Sets of shingles are compressed into compact, fixed-length signatures (e.g., 100 dimensions) that preserve Jaccard similarity.
* LSH Indexing: Signatures are divided into **b** bands of **r** rows. Documents that share an identical sub-signature in any band are hashed into the same bucket.
* Verification: For every query, we only compute the exact Jaccard similarity for candidates found in the same LSH buckets, ensuring high precision with minimal computation.

# Collected dataset
Our dataset is built from three sources: **WikiEdit pairs** from HuggingFace, **IELTS writing samples** from Kaggle, and a collection of documents from a Near-Duplicate Detection competition on Kaggle. This provides a mix of academic essays, short edits, and complex technical texts to ensure the algorithm is tested against varied writing styles.

The dataset is divided into two primary components: the __Original Library__ and the __Target Suite__. The Original Library serves as our reference database, containing unique source documents. The Target Suite is composed of variants generated from these sources, designed to test the algorithm's detection capabilities across different levels of modification. We utilized the __PAN-09 dataset__ to introduce 'noise' documents into the library. These are files with no true matches in our query set, specifically included to measure the False Positive Rate. This ensures the system maintains high precision by effectively ignoring unrelated data.

Below is the data spectrum of the dataset used:

    Original lib (about 1k) : 80% related, 20% unrelated (random noise)
        Only unique items

    Target suite (about 5k) :
        * Tiers: 10% identical (J = 1.0), 40% heavy/medium edits (J : [0.6; 0.8)), 30% light edit (J : [0.8; 1)), 20% non-duplicate (J [0; 0.2))
        * Length: 20% short ( <= 200 char), 60% medium (<= 5k chars), 20% long (<= 1000k chars)

__Source dataset links:__
* Wiki Edits: https://huggingface.co/datasets/zhk/wiki-edits
* IELTS writing task 2 samples: https://www.kaggle.com/datasets/mazlumi/ielts-writing-scored-essays-dataset
* Near-Duplicate Detection competition on Kaggle: https://www.kaggle.com/competitions/near-duplicates/overview
* PAN-09: https://webis.de/data/pan-pc-09.html

__Download our dataset here:__ https://drive.google.com/drive/u/3/folders/1qliLoTttYJwkqzlzCbqvU9htnOfwJ62z

# Performance metrics
To validate the efficiency and accuracy of the LSH approach, we track the following key metrics against the Brute Force baseline:
* Precision@Threshold: 
* Recovery Rate (Recall): The percentage of ground-truth duplicates successfully retrieved.
* Selectivity: The ratio of the library actually compared (aiming for $< 1\%$).
* Mean Reciprocal Rank (MRR): Evaluates if the correct source is the top-ranked result.
* Average query time: Total time taken to process a single document from the target suite.

# Experiment results
## Prerequisites
* C++17 compatible compiler (GCC/Clang)

* nlohmann/json for dataset parsing (https://github.com/nlohmann/json)

## Build and Run
To evaluate the performance of the system, you can choose between the **Brute Force** baseline and the **LSH-optimized** approach.

1. ### Configure Parameters:
    Navigate to the test folder and open the benchmark file. Adjust the hyperparameters using the following method:

    ```
    // Format: setConfig(k_gram, bands, rows, threshold)
    C++Config::setConfig(k_gram, bands, rows, threshold);
    ```

* **k_gram**: Size of character shingles.
* **bands/rows**: LSH sub-signature configuration ($b \times r = \text{total hash functions}$).
* **threshold**: The Jaccard similarity target (e.g., 0.7).

2. ### Execute the Benchmark:
    Run the ```.bat``` file corresponding to the method you wish to test. This will compile the source and initiate the benchmark automatically.
* For Brute Force Baseline:
    ```./bruteforce_build.bat```
* For LSH Performance:
    ```./lsh_build.bat```

**View Results**: Execution results will be logged to a ```txt.```

3. ### Results

| Config [k, b, r] | Recovery Rate | Precision@0.7 | MAE | Selectivity | Total runtime (ms) |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **Brute Force** | __100%__ | __100%__| __0.000__ | 100% | 1.67523e+07 |
| **LSH [5, 20, 5]** | 88.2% | 84.5% | 2.1% | 0.45% | __313992__ |
| **LSH [5, 14, 7]** | 78.4% | __89.1%__ | __1.8%__ | __0.08%__ | 315236 |
| **LSH [5, 40, 4]** | 90.1% | 66.2% | 3.5% | 1.20% | 332930 |
| **LSH [4, 20, 5]** | __95.2%__ | 22.4% | 4.8% | 0.47% | 322120 |
| **LSH [4, 25, 4]** | 95.1% | 7.68% | 4.8% | 1.39% | 1.35423e+06 |
| **LSH [4, 25, 8]** | 81.8% | 68.9% | 2.0% | 0.13% | 409511 |
| **LSH [4, 14, 7]** | 81.8% | 61.5% | 3.66% | 0.14% | 349026 |
| **LSH [4, 50, 4]** | 95.1% | 4.94% | 2.88% | 2.16% | 1.94197e+06 |
| **LSH [4, 30, 5]** | 95.1% | 17.8% | 3.78% | 0.60% | 1.49115e+06 |

*Tests conducted on a dataset of ~5,000 documents. Threshold target: 0.7. This output is what we were able to collect and is subjective to parameter tuning.*