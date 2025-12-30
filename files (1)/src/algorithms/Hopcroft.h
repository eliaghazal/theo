#pragma once
#include "../model/DFA.h"
#include <vector>
#include <string>
#include <set>

struct HopcroftSplit {
    std::set<std::string> Y;
    std::set<std::string> Y_intersect_X;
    std::set<std::string> Y_minus_X;
};

struct HopcroftStep {
    std::vector<std::set<std::string>> partitionsBefore;
    std::vector<std::set<std::string>> partitionsAfter;
    std::set<std::string> A; // The splitter set chosen from W
    std::string symbol;      // The symbol c
    std::set<std::string> X; // Preimage of A under c
    std::vector<HopcroftSplit> splits;
    std::string explanation;
};

class Hopcroft {
public:
    static std::pair<DFA, std::vector<HopcroftStep>> minimize(const DFA& inputDFA);
};