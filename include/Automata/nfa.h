#ifndef __NFA_H__
#define __NFA_H__

#include "Automata/dfa.h"
#include <vector>
#include <string>
#include <type_traits>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <iterator>

using namespace std;



// Serialization function that serializes an array or a set
template<class T, 
		typename std::enable_if<
		    std::is_same<std::vector<std::string>,T>::value 
		    || std::is_same<std::set<std::string>, T>::value, T>::type* = nullptr>
std::string serialize(T obj);



// This is a non-determimistic general case of the fininte automata
class NFA : public DFA {
    public:
        using DFA::epsilon;


        // Override of constructTransition that handles the special treatment required for NFAs 
        void constructTransition(string stateID_from, string stateID_to, char letter) override;
        // Function which converts this current NFA into an DFA using the epsilon-closure method
        DFA epsilonClosureConvert();

    private:
        // Determines the e-closure for a specific set of states
        vector<string> epsilonClosure(vector<string> states);
        // Converts a raw DFA tables into a plain DFA
        DFA rawDFAtoDFA(map<string, map<char, string>> rawDFA, vector<string> insertionOrder);
        // Specific to the DFA this is just a function that returns all the possible places a specific state can take us
        vector<string> possibleDestinations(string state, char letter);


    private:
        using DFA::accepts;
};


#endif // __NFA_H__