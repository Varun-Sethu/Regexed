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

    protected:
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
        string addState(bool accepting);
        // Add a letter to the DFA's alphapbet
        void addAlphabet(char letter);
        // Construct a transition function between two states given the element of the alphabet that is meant to be consumed
        virtual void constructTransition(string stateID_from, string stateID_to, char letter);
        // Utility functions
        // Determines weather a specific sentence is valid in the FSM's language
        virtual bool accepts(string sentence);

        // De-registers a state as a terminating state
        bool de_register_terminating_state(std::string state) {
            // Simply just remove it from the accepting states set
            collection::iterator pos = acceptingStates.find(state);
            if (pos != acceptingStates.end()) 
                acceptingStates.erase(pos);
        } 

        // Operators
        DFA& operator=(const DFA& other) {
            acceptingStates = other.acceptingStates;
            states = other.states;
            alphabet = other.alphabet;
            transitionFunctions = other.transitionFunctions;
        }
    private:
        // verifies that this DFA is a valid DFA
        bool valid_DFA();

    protected:
        const char epsilon = static_cast<char>(108);

        // Converts a state to an integer (the integer is the number of the state that is the number it was assigned when it was set up)
        int stateasInt(string stateID);

};



#endif // __FA_H__