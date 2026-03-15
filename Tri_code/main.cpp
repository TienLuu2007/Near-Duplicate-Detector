#include "function.h"
#include <fstream>

int main()
{
    std::ifstream fin("input.txt");
    std::ofstream fout("output.txt");
    if(fin.is_open() and fout.is_open())
    {
        std::vector<std::string> s;

        std::string line;
        while(std::getline(fin, line))
        {
            if(!line.empty())
            {
                s.push_back(line);
            }
        }
        
        int n = s.size();
        std::vector<std::vector<std::string>> tokens(n);
        std::vector<std::unordered_set<std::string>> shingles(n);
        std::vector<std::vector<uint32_t>> sig(n);
        
        for (int i = 0; i < n; i++)
        {
            s[i] = normalize(s[i]);
            tokenize(s[i], tokens[i]);
            shingle(tokens[i], k, shingles[i]);
            sig[i] = Minhash(shingles[i]);
            
            // for (uint32_t val : sig[i])
            //     fout << val << " ";
            // fout << "\n";
            
            if (i >= 1)
            {
                fout << std::fixed << std::setprecision(2) 
                        << Jaccard(sig[0], sig[i]) << "\n";
            }
        }
        
        fout << "|-|done|-|";
    }
    fin.close();
    fout.close();
    
}