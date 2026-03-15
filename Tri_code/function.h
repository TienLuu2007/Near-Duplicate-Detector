#ifndef FUNCTION_H
#define FUNCTION_H

#include <bits/stdc++.h>

const int sseed = 100;
const int k = 3;

// Function declarations
std::string normalize(std::string& s);
void tokenize(std::string s, std::vector<std::string>& tokens);
void shingle(std::vector<std::string> tokens, int k, std::unordered_set<std::string>& shingles);
uint32_t hashfunction(const std::string& shingle, uint32_t seed);
std::vector<uint32_t> Minhash(std::unordered_set<std::string> shingles);
double Jaccard(std::vector<uint32_t> sig0, std::vector<uint32_t> sig);

#endif