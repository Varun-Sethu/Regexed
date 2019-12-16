#ifndef __DFA_H__
#define __DFA_H__


#include <map>
#include <string>
#include <algorithm>
#include <set>
#include <utility>

using namespace std;
typedef set<string> collection;


// Finite automata structure
class DFA {
    public:
        string initialState;

    //protected:
        // Core data and information
        collection states;
        collection acceptingStates;
        set<char> alphabet;
        map<string, 
                multimap<char, string>> transitionFunctions;


    public:
        DFA(set<char> alphabet = set<char>()) 
            : alphabet(alphabet) 
        {
            
        }



        // Adds a state to the DFA and returns the state id for it
        string addState(bool accepting) {
            string id = "s-" + to_string(states.size());
            if (accepting) {
                id = "t" + id;
                acceptingStates.insert(id);
            }

            // Construct an entry for this new state into the transitionFunctions list
            transitionFunctions.insert(
                    make_pair(id, multimap<char, string>())); 
            states.insert(id);
            // If the size of the states set is 1 then the state just inserted is an initial state
            if (states.size() == 1) initialState = id;
            return id;
        }



        
        // Add a letter to the DFA's alphapbet
        void addAlphabet(char letter) {
            alphabet.insert(letter);
        }





        // Construct a transition function between two states given the element of the alphabet that is meant to be consumed
        virtual void constructTransition(string stateID_from, string stateID_to, char letter) {
            // If the letter is not in the alphabet or the states do not exist then cancel the construction
            if(stateasInt(stateID_to) >= states.size() 
                || stateasInt(stateID_from) >= states.size() 
                || !alphabet.count(letter)) return;

            // Ensure that no existing transition function exists from this state to another via the same letter
            const bool noTransitionExists = transitionFunctions[stateID_from]
                                                        .count(letter) == 0;
            // Only construct this new transition if no such functions existed previously
            if (!noTransitionExists) return;
            else 
                transitionFunctions[stateID_from].insert(make_pair(
                                                        letter, stateID_to));
        }





        // Utility functions
        // Determines weather a specific sentence is valid in the FSM's language
        virtual bool accepts(string sentence) {
            if (!valid_DFA()) return false;

            string currentState = initialState;
            // continue running this while there are still letters to be consumed
            while (sentence.length() > 0) {
                map<char, string> moveFunctions{ transitionFunctions[currentState].begin(), 
                                                           transitionFunctions[currentState].end() };

                // If possible consume the next letter in the sentence and progress the state
                auto tryConsumingLetter = [&](pair<char, string> function){
                    bool canConsumeSentence = function.first == static_cast<char>(sentence[0]);
                    if (canConsumeSentence) {
                        currentState = function.second;
                        // Trim the current sentence by consuming the first letter
                        sentence = sentence.substr(1, sentence.length());
                        return true;
                    }
                    return false;
                };
                // Iterate over each possible move function and consume the next letter
                bool consumedSentence = any_of(moveFunctions.begin(), moveFunctions.end(), tryConsumingLetter);
                
                // If there were no possible functions from this state to another then terminate to match as we are now in a dead state
                if (!consumedSentence) return false;
            }

            // If we ended on a terminating state that means the sentence was accepted by the DFA
            return acceptingStates.count(currentState) > 0;
        }




    private:
        // verifies that this DFA is a valid DFA
        bool valid_DFA() {
            auto valid = [&](pair<string, multimap<char, string>> transFunctions) {
                // Special exception for terminating states
                if (this->acceptingStates.count(transFunctions.first) > 0) return true;
                return transFunctions.second.size() == this->alphabet.size();
            }; // ensures that the current state being tested accepts the entire alphabet

            return all_of(transitionFunctions.begin(), transitionFunctions.end(), valid);
        }



    protected:
        const char epsilon = static_cast<char>(108);

        // Converts a state to an integer (the integer is the number of the state that is the number it was assigned when it was set up)
        int stateasInt(string stateID) {
            return stoi(stateID.substr(stateID.find("-") + 1));
        }

};



#endif // __FA_H__