#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace Config 
{
    // --------- Default Settings --------- //
    // Shingle size (k-gram)
    inline int K_GRAM = 4;
    // LSH Parameters
    inline int BANDS = 20;
    inline int ROWS = 5;
    inline int SIGNATURE_SIZE = BANDS * ROWS;
    // Similarity threshold
    inline double THRESHOLD = 0.7;

    inline void setConfig(int k, int b, int r, double t) 
    {
        K_GRAM = k;
        BANDS = b;
        ROWS = r;
        SIGNATURE_SIZE = b * r;
        THRESHOLD = t;
    }
}

#endif