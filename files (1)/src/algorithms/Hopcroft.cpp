#include "Hopcroft.h"
#include <iostream>
#include <algorithm>
#include <sstream>

// Helper to check set equality
bool setsEqual(const std::set<std::string>& a, const std::set<std::string>& b) {
    return a == b;
}

std::pair<DFA, std::vector<HopcroftStep>> Hopcroft::minimize(const DFA& inputDFA) {
    DFA dfa = inputDFA;
    
    // Preprocessing: Remove unreachable states
    // Note: Completeness should ideally be handled before this call by the user or auto-sink.
    dfa.removeUnreachable(); 
    
    std::set<std::string> F, Q_minus_F;
    std::set<std::string> allStates;
    
    for (const auto& s : dfa.states) {
        allStates.insert(s.id);
        if (s.isAccept) F.insert(s.id);
        else Q_minus_F.insert(s.id);
    }
    
    std::vector<std::set<std::string>> P;
    if (!F.empty()) P.push_back(F);
    if (!Q_minus_F.empty()) P.push_back(Q_minus_F);
    
    // W = {F} if |F| <= |Q\F| else {Q\F}. 
    // Actually standard Hopcroft initializes W = {F, Q\F} is also fine, 
    // or typically just {F} is enough to start if F is smaller.
    // Let's stick to standard Algorithm: W = P
    std::vector<std::set<std::string>> W = P;
    
    std::vector<HopcroftStep> history;
    
    while (!W.empty()) {
        std::set<std::string> A = W.back();
        W.pop_back();
        
        for (const auto& c : dfa.alphabet) {
            // X = { q | delta(q,c) in A }
            std::set<std::string> X;
            for (const auto& s : dfa.states) {
                if (dfa.transitions.count({s.id, c})) {
                    std::string dest = dfa.transitions.at({s.id, c});
                    if (A.count(dest)) {
                        X.insert(s.id);
                    }
                }
            }
            
            if (X.empty()) continue;

            HopcroftStep step;
            step.partitionsBefore = P;
            step.A = A;
            step.symbol = c;
            step.X = X;
            
            std::vector<std::set<std::string>> nextP;
            bool anySplit = false;
            
            for (const auto& Y : P) {
                std::set<std::string> intersection;
                std::set<std::string> difference;
                
                std::set_intersection(Y.begin(), Y.end(), X.begin(), X.end(),
                                      std::inserter(intersection, intersection.begin()));
                std::set_difference(Y.begin(), Y.end(), X.begin(), X.end(),
                                    std::inserter(difference, difference.begin()));
                
                if (!intersection.empty() && !difference.empty()) {
                    // Split Y into intersection and difference
                    nextP.push_back(intersection);
                    nextP.push_back(difference);
                    
                    HopcroftSplit splitInfo;
                    splitInfo.Y = Y;
                    splitInfo.Y_intersect_X = intersection;
                    splitInfo.Y_minus_X = difference;
                    step.splits.push_back(splitInfo);
                    
                    // Update W
                    // If Y is in W, replace Y with intersection and difference
                    auto itW = std::find(W.begin(), W.end(), Y);
                    if (itW != W.end()) {
                        W.erase(itW);
                        W.push_back(intersection);
                        W.push_back(difference);
                    } else {
                        // If not in W, add the smaller one
                        if (intersection.size() <= difference.size()) {
                            W.push_back(intersection);
                        } else {
                            W.push_back(difference);
                        }
                    }
                    anySplit = true;
                } else {
                    nextP.push_back(Y);
                }
            }
            P = nextP;
            step.partitionsAfter = P;
            
            if (anySplit) {
                 std::stringstream ss;
                 ss << "Refining partition with respect to set A and symbol '" << c << "'. "
                    << step.splits.size() << " blocks were split.";
                 step.explanation = ss.str();
                 history.push_back(step);
            }
        }
    }
    
    // Construct minimized DFA
    DFA minDFA;
    minDFA.alphabet = dfa.alphabet;
    
    // Create states for each block
    int blockIdx = 0;
    std::map<std::string, std::string> oldToNew; // oldId -> newId
    
    // Calculate somewhat smart positions
    double centerX = 400;
    double centerY = 300;
    double radius = 200;
    int totalBlocks = P.size();

    for (const auto& block : P) {
        std::string newId = "{" + (*block.begin()) + (block.size()>1 ? ".." : "") + "}";
        // Simplified naming or just q0', q1' could be better but let's use list rep for debug
        // For cleaner ID, let's use q_0, q_1...
        // But let's try to preserve meaningful names if possible.
        // Actually, let's just name them Q0, Q1...
        std::string finalId = "Q" + std::to_string(blockIdx);
        
        bool isStart = false;
        bool isAccept = false;
        
        for (const auto& oldId : block) {
            const DFAState* s = dfa.getState(oldId);
            if (s && s->isStart) isStart = true;
            if (s && s->isAccept) isAccept = true;
            oldToNew[oldId] = finalId;
        }
        
        double angle = (2 * 3.14159 * blockIdx) / totalBlocks;
        minDFA.addState(finalId, isStart, isAccept, 
                        centerX + radius * cos(angle), 
                        centerY + radius * sin(angle));
        blockIdx++;
    }
    
    // Add transitions
    // Since it's a partition, taking any representative from block is enough
    for (const auto& block : P) {
        if (block.empty()) continue;
        std::string rep = *block.begin(); // Representative
        std::string fromNew = oldToNew[rep];
        
        for (const auto& c : minDFA.alphabet) {
            if (dfa.transitions.count({rep, c})) {
                std::string toOld = dfa.transitions.at({rep, c});
                std::string toNew = oldToNew[toOld];
                minDFA.addTransition(fromNew, c, toNew);
            }
        }
    }
    
    return {minDFA, history};
}