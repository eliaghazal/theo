#include <QApplication>
#include "MainWindow.h"
#include "model/DFA.h"
#include "algorithms/Hopcroft.h"
#include <iostream>
#include <cassert>

// Simple logic test
void runTests() {
    std::cout << "Running Logic Tests..." << std::endl;
    DFA dfa;
    dfa.alphabet = {"a", "b"};
    dfa.addState("q0", true, false, 0, 0);
    dfa.addState("q1", false, true, 0, 0);
    dfa.addTransition("q0", "a", "q1");
    dfa.addTransition("q0", "b", "q0");
    dfa.addTransition("q1", "a", "q1");
    dfa.addTransition("q1", "b", "q0");
    
    auto res = Hopcroft::minimize(dfa);
    // q0 and q1 distinguish by accept status immediately.
    // They are distinguishable. Size should be 2.
    assert(res.first.states.size() == 2);
    std::cout << "Test 1 Passed." << std::endl;
    
    // Redundant state test
    DFA dfa2;
    dfa2.alphabet = {"a"};
    dfa2.addState("A", true, false, 0, 0);
    dfa2.addState("B", false, false, 0, 0);
    dfa2.addTransition("A", "a", "B");
    dfa2.addTransition("B", "a", "A");
    // Both non-accepting, symmetric. Should merge into 1 state if we consider them equivalent?
    // Wait, if neither accepts, they are equivalent.
    auto res2 = Hopcroft::minimize(dfa2);
    assert(res2.first.states.size() == 1);
    std::cout << "Test 2 Passed." << std::endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Check for test flag
    if (argc > 1 && std::string(argv[1]) == "--test") {
        runTests();
        return 0;
    }

    MainWindow w;
    w.show();
    return app.exec();
}