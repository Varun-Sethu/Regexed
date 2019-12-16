#include "Automata/dfa.h"
#include "Automata/nfa.h"
#include <string>
#include <set>
#include <iostream>

using namespace std;


int main(void) {
    NFA nfa;
    
    nfa.addAlphabet('0');
    nfa.addAlphabet('1');

    string st0 = nfa.addState(false);
    string st1 = nfa.addState(false);
    string st2 = nfa.addState(true);

    nfa.constructTransition(st0, st2, '0');

    nfa.constructTransition(st2, st0, '1');
    nfa.constructTransition(st2, st0, '0');

    nfa.constructTransition(st1, st0, '1');
    
    nfa.constructTransition(st1, st2, '1');
    nfa.constructTransition(st2, st1, '0');

    auto dfa = nfa.epsilonClosureConvert();

    std::cout << "hello";
}   