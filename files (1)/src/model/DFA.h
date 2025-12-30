#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

struct DFAState {
    std::string id;
    bool isStart = false;
    bool isAccept = false;
    // Layout hints
    double x = 0.0;
    double y = 0.0;
};

struct DFATransition {
    std::string from;
    std::string symbol;
    std::string to;
};

class DFA {
public:
    std::vector<std::string> alphabet;
    std::vector<DFAState> states;
    // Map: pair(from_id, symbol) -> to_id
    std::map<std::pair<std::string, std::string>, std::string> transitions;

    void addState(const std::string& id, bool start, bool accept, double x, double y);
    void addTransition(const std::string& from, const std::string& symbol, const std::string& to);
    
    // Validation
    bool isDeterministic() const;
    std::vector<std::pair<std::string, std::string>> getMissingTransitions() const;
    
    // Manipulation
    void removeUnreachable();
    void completeWithSink();
    
    DFAState* getState(const std::string& id);
    const DFAState* getState(const std::string& id) const;
    
    // Helpers
    std::set<std::string> getReachableStates() const;
    void clear();
};