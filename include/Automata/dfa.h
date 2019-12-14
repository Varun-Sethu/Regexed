#ifndef __DFA_H__
#define __DFA_H__


#include <vector>
#include <unordered_map>
#include <string>
#include <tuple>
#include <algorithm>
#include <utility>

using namespace std;

// Finite automata structure
class DFA {
    public:
        // Core data and information
        vector<string> states;
        vector<char> alphabet;
        unordered_map<string, vector<pair<char, string>>> transitionFunctions;
        string initialState;
        vector<string> acceptingStates;


    public:
        DFA() {
            this->initialState = "s-0";
            this->states.push_back("s-0");
        }


        // Adds a state to the DFA and returns the state id for it
        string addState(bool isAccepting) {
            string id = "s-" + to_string(states.size());
            if (isAccepting) {
                id = "t" + id;
                acceptingStates.push_back(id);
            }
            this->states.push_back(id);
            return id;
        }



        
        // Add a letter to the DFA's alphapbet
        void addAlphabet(char letter) {
            if (std::find(alphabet.begin(), alphabet.end(), letter) != alphabet.end() || letter == this->epsilon) return;
            this->alphabet.push_back(letter);
        }





        // Construct a transition function between two states
        virtual void constructTransition(string stateID_from, string stateID_to, char letter) {
            // Determine if these objects actually exist in each of the vectors
            if(stoi(stateID_to.substr(stateID_to.find("-") + 1)) >= states.size() || stoi(stateID_from.substr(stateID_from.find("-") + 1)) >= states.size() || find(alphabet.begin(), alphabet.end(), letter) == alphabet.end()) {
                return;
            }
            if (transitionFunctions.find(stateID_from) == transitionFunctions.end()) {
                transitionFunctions.insert(make_pair(stateID_from, vector<pair<char, string>>())); 
            }


            // Before inserting we must ensure that a transition function like this doesn't actually exist in the first place
            for(auto p: transitionFunctions[stateID_from]) {
                if (p.first == letter) return;
            }
            transitionFunctions[stateID_from].push_back(make_pair(letter, stateID_to));
        }





        // Utility functions
        // Determines weather a specific sentence is valid in the FSM's language
        virtual bool accepts(string sentence) {
            if (!verify()) return false;
            string currentState = this->initialState;

            // continue running this while it is not a termial state
            while (sentence.length() != 0) {
                // Attain a list of all the transition functions from this current state
                if (transitionFunctions.find(currentState) == transitionFunctions.end()) return false;
                vector<pair<char, string>> moveFunctions{transitionFunctions[currentState].begin(), transitionFunctions[currentState].end()};

                // Iterate over each of these transition functions until we approach the first function that well accept this alphabet
                for(pair<char, string> function: moveFunctions) {
                    if (function.first == (char)sentence[0]) {
                        currentState = function.second;
                        // Trim the sentence
                        sentence = sentence.substr(1, sentence.length());
                    }
                }
            }
            return currentState[0] == 't';
        }

    private:

        // verifies that this DFA is actuall an DFA
        bool verify() {
            for(auto it = transitionFunctions.begin(); it != transitionFunctions.end(); it++) {
                vector<pair<char, string>> moveFunctions = it->second;
                if (moveFunctions.size() != alphabet.size()) return false;
            }
            return true;
        }



    protected:
        const char epsilon = static_cast<char>(108);


        vector<string> possibleDestinations(string state, char letter) {
            vector<string> candidates;
        
            for (pair<char, string> function: transitionFunctions[state]) {
                if (function.first == letter) candidates.push_back(function.second);
            }
        
            return candidates;
        }

};



#endif // __FA_H__