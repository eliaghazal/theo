#include "DFA.h"
#include <queue>
#include <algorithm>

void DFA::addState(const std::string& id, bool start, bool accept, double x, double y) {
    // If start is true, unset others
    if (start) {
        for (auto& s : states) s.isStart = false;
    }
    states.push_back({id, start, accept, x, y});
}

void DFA::addTransition(const std::string& from, const std::string& symbol, const std::string& to) {
    transitions[{from, symbol}] = to;
}

DFAState* DFA::getState(const std::string& id) {
    for (auto& s : states) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

const DFAState* DFA::getState(const std::string& id) const {
    for (const auto& s : states) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

bool DFA::isDeterministic() const {
    // In this model, using a map automatically prevents duplicate (state, symbol) keys.
    // However, we must ensure every state has exactly one transition for every symbol
    // to be "fully" deterministic in some definitions, but usually valid input
    // just requires <= 1. If map logic enforces 1, we check if we have all symbols.
    // Here we define deterministic as: No ambiguity. Map structure enforces this naturally.
    return true; 
}

std::vector<std::pair<std::string, std::string>> DFA::getMissingTransitions() const {
    std::vector<std::pair<std::string, std::string>> missing;
    for (const auto& s : states) {
        for (const auto& sym : alphabet) {
            if (transitions.find({s.id, sym}) == transitions.end()) {
                missing.push_back({s.id, sym});
            }
        }
    }
    return missing;
}

std::set<std::string> DFA::getReachableStates() const {
    std::set<std::string> reachable;
    std::queue<std::string> q;
    
    std::string startId;
    for (const auto& s : states) {
        if (s.isStart) {
            startId = s.id;
            break;
        }
    }
    
    if (startId.empty()) return reachable;
    
    reachable.insert(startId);
    q.push(startId);
    
    while (!q.empty()) {
        std::string curr = q.front();
        q.pop();
        
        for (const auto& sym : alphabet) {
            if (transitions.count({curr, sym})) {
                std::string next = transitions.at({curr, sym});
                if (reachable.find(next) == reachable.end()) {
                    reachable.insert(next);
                    q.push(next);
                }
            }
        }
    }
    return reachable;
}

void DFA::removeUnreachable() {
    std::set<std::string> reachable = getReachableStates();
    std::vector<DFAState> newStates;
    for (const auto& s : states) {
        if (reachable.count(s.id)) {
            newStates.push_back(s);
        }
    }
    states = newStates;
    
    // Clean transitions
    auto it = transitions.begin();
    while (it != transitions.end()) {
        if (reachable.find(it->first.first) == reachable.end() ||
            reachable.find(it->second) == reachable.end()) {
            it = transitions.erase(it);
        } else {
            ++it;
        }
    }
}

void DFA::completeWithSink() {
    auto missing = getMissingTransitions();
    if (missing.empty()) return;
    
    // Create sink state
    std::string sinkName = "sink";
    int i = 0;
    while(getState(sinkName)) {
        sinkName = "sink" + std::to_string(++i);
    }
    
    addState(sinkName, false, false, 0, 0); // Position at 0,0, user can move
    
    // Add transitions to sink
    for (const auto& m : missing) {
        addTransition(m.first, m.second, sinkName);
    }
    
    // Sink loops
    for (const auto& sym : alphabet) {
        addTransition(sinkName, sym, sinkName);
    }
}

void DFA::clear() {
    states.clear();
    transitions.clear();
    alphabet.clear();
}