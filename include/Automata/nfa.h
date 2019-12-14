#ifndef __NFA_H__
#define __NFA_H__

#include "Automata/dfa.h"
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <queue>
#include <set>
#include <utility>

using namespace std;


// TODO: Remove repetiveness of code


// This is a non-determimistic general case of the fininte automata
class NFA : public DFA {
    public:
        using DFA::epsilon;

        void constructTransition(string stateID_from, string stateID_to, char letter) override {
            // Determine if these objects actually exist in each of the vectors
            if(stoi(stateID_to.substr(stateID_to.find("-") + 1)) >= states.size() || stoi(stateID_from.substr(stateID_from.find("-") + 1)) >= states.size() || find(alphabet.begin(), alphabet.end(), letter) == alphabet.end()) {
                // This is a bit hacky, ill fix it later :P
                if (letter != epsilon) return;
            }
            if (transitionFunctions.find(stateID_from) == transitionFunctions.end()) {
                transitionFunctions.insert(make_pair(stateID_from, vector<pair<char, string>>())); 
            }
            transitionFunctions[stateID_from].push_back(make_pair(letter, stateID_to));
        }






        // Function which converts this current NFA into an DFA using the epsilon-closure method
        DFA epsilonClosureConvert() {
            // Stack of states that must be generate test
            queue<vector<string>> toGenerateStates;
            toGenerateStates.push(epsilonClosure({initialState}));
            // A raw representation of the DFA as it stands
            map<vector<string>, vector<pair<char, vector<string>>>> rawDFA;

            while (toGenerateStates.size() != 0) {
                // Begin the process of generation for the most recent object on the stack
                vector<string> currentState = toGenerateStates.front();
                toGenerateStates.pop();
                // There was a bug earlier that i couldn't seem to get around this is just a quick hack
                if (rawDFA.count(currentState)) continue;


                // Insert this new state into the raw DFA in order to prevent multiple computation of the e-closure
                rawDFA.insert(make_pair(currentState, vector<pair<char, vector<string>>>()));
                auto dictRef = rawDFA.find(currentState);


                // The full list of transition functions to be generated for this specific state
                vector<pair<char, vector<string>>> transitionFunctions;

                // Generate the transition function for each specific alphabet, aka generate all the possible transitions from this state
                for (char letter: alphabet) {
                    set<string> possibleTransitions;

                    for (auto state: currentState) {
                        auto localTransitions = possibleDestinations(state, letter);
                        copy(localTransitions.begin(), 
                            localTransitions.end(), 
                            inserter(possibleTransitions, 
                                possibleTransitions.end()));
                    }

                    // From the set of possible transition vectors compute the epsilon-closure, this is the "output" of the transition function within the DFA
                    vector<string> outputState = 
                        epsilonClosure({ possibleTransitions.begin(), possibleTransitions.end() });

                    // If this doesn't output state already exists as a generated state then push it into the toGenerateStates queue
                    if (!rawDFA.count(outputState)) 
                        toGenerateStates.push(outputState);
                        
                    transitionFunctions.push_back(make_pair(letter, outputState));
                }
                
                // Insert this new generated set of transition functions into the rawDFA
                dictRef->second.insert(dictRef->second.end(), transitionFunctions.begin(), transitionFunctions.end());
            }

            return rawDFAtoDFA(rawDFA, alphabet, acceptingStates);
        }










    private:
        // Determines the e-closure for a specific set of states
        vector<string> epsilonClosure(vector<string> states) {
            vector<string> candidateList(states.begin(), states.end());

            for(string state: states) {
                // Attain a full list of transitions
                vector<pair<char, string>> transitionFunctions = this->transitionFunctions[state];
                // Iterate over each of them and only include states in which the transition occurs on an epsilon
                for(auto transitionPair: transitionFunctions) {
                    if (transitionPair.first == this->epsilon) candidateList.push_back(transitionPair.second);
                }
            }

            return candidateList;
        }



        // Converts a raw DFA tables int
        DFA rawDFAtoDFA(map<vector<string>, vector<pair<char, vector<string>>>> rawDFA, vector<char> alphabet, vector<string> initialAcceptingStates) {
            // Map containing all the state ids and their respective states
            map<vector<string>, string> stateIDtoState;
            DFA dfa;
            dfa.alphabet = alphabet;

            // First pass to register all the states in the DFA
            for(auto it = rawDFA.begin(); it != rawDFA.end(); it++) {
                vector<string> stateCop = it->first;

                // Determine if the state that we are checking actually is an accepting state 
                bool isAcceptingState = find_if(stateCop.begin(), stateCop.end(), [&](string entry){
                    for (string accStates: initialAcceptingStates) {
                        if (accStates == entry) return true;
                    }
                    return false;
                }) != stateCop.end();
                string stateId = dfa.addState(isAcceptingState);
                stateIDtoState.insert(make_pair(stateCop, stateId));
            }

            // Second pass to actually connect the nods together through transition functions
            for (auto it = rawDFA.begin(); it != rawDFA.end(); it++) {
                string stateID = stateIDtoState[it->first];
                for (pair<char, vector<string>> transitionFunction: it->second) {
                    dfa.constructTransition(stateID, stateIDtoState[transitionFunction.second], transitionFunction.first);
                }
            }

            return dfa;
        }


    private:
        using DFA::accepts;
};


#endif // __NFA_H__