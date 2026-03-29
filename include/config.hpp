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
        if(k >= 1) K_GRAM = k;
        if(b >= 1)
        {
            BANDS = b;
            SIGNATURE_SIZE = b * r;
        } 
        if(r >= 1)
        {
            ROWS = r;
            SIGNATURE_SIZE = b * r;
        }
        if(t >= 0 && t <= 1) THRESHOLD = t;
    }

    inline void setThreshold(double t) 
    {
        if(t >= 0 && t <= 1) THRESHOLD = t;
    }

    inline void setKGram(int k) 
    {
        if(k >= 1) K_GRAM = k;
    }
}

#endif