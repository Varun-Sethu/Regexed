#include "Automata/dfa.h"
#include "Automata/nfa.h"
#include <string>
#include <iostream>

using namespace std;


int main(void) {
    NFA nfa;

    string stateOne = nfa.addState(false);
    string stateTwo = nfa.addState(false);
    string stateThree = nfa.addState(true);

    nfa.addAlphabet('a');
    nfa.addAlphabet('b');
    nfa.addAlphabet('c'); 

    nfa.constructTransition(nfa.initialState, stateOne, nfa.epsilon);
    nfa.constructTransition(nfa.initialState, stateTwo, 'a');
    nfa.constructTransition(stateOne, stateTwo, 'b');
    nfa.constructTransition(stateTwo, stateTwo, 'b');
    nfa.constructTransition(stateTwo, stateThree, 'c');
    nfa.constructTransition(stateOne, stateThree, 'c');

    DFA convertedDfa = nfa.epsilonClosureConvert();
}   