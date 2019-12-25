#include "Automata/nfa.h"

using namespace std;



// UTIL
template<class T, 
		typename std::enable_if<
		    std::is_same<std::vector<std::string>,T>::value 
		    || std::is_same<std::set<std::string>, T>::value, T>::type* = nullptr>
std::string serialize(T obj) {
    std::ostringstream os;
	std::copy(obj.begin(), obj.end(), std::ostream_iterator<std::string>(os));
	return os.str();
}



// NFA Stuff


// ___PUBLIC___

// Override of constructTransition that handles the special treatment required for NFAs 
void NFA::constructTransition(string stateID_from, string stateID_to, char letter) {
    // If the letter is not in the alphabet or the states do not exist then cancel the construction
    if(stateasInt(stateID_to) >= states.size() 
        || stateasInt(stateID_from) >= states.size() 
        || (!alphabet.count(letter)) 
            && letter != epsilon) return;

    transitionFunctions[stateID_from].insert(make_pair(
                                            letter, stateID_to));
}

 // Function which converts this current NFA into an DFA using the epsilon-closure method
DFA NFA::epsilonClosureConvert() {
    // Stack of states that must be generate test
    queue<vector<string>> toGenerateStates;
    toGenerateStates.push(
        epsilonClosure({ initialState }));
    // A raw representation of the DFA as it stands
    map<string, 
            map<char, string>> rawDFA;
    // Bit of a hack... tracks insertion order making it easier for the constructDFA function
    vector<string> insertionOder;


    // While there still exists more stats to generate continue running in this loop
    while (toGenerateStates.size() > 0) {
        // Begin the process of generation for the most recent object on the stack
        vector<string> currentState = toGenerateStates.front();
        toGenerateStates.pop();
        string serializedState = serialize(currentState);
        if (rawDFA.count(serializedState) > 0) continue;

        // Insert this new state into the raw DFA in order to prevent multiple computation of the e-closure
        rawDFA.insert(make_pair(serializedState, 
                                map<char, string>()));

        // The full list of transition functions to be generated for this specific state
        map<char, string> transitionFunctions;

        // Generate the transition function for each specific alphabet, aka generate all the possible transitions from this state
        for (char letter: alphabet) {
            set<string> possibleTransitions;

            // Compute the transitions for each state for this specific letter
            auto computeTransitions = [&](string state) {
                auto localTransitions = possibleDestinations(state, letter);
                copy(localTransitions.begin(), 
                    localTransitions.end(), 
                    inserter(possibleTransitions, 
                        possibleTransitions.end()));
            };
            for_each(currentState.begin(), currentState.end(), computeTransitions);
            vector<string> outputState = 
                epsilonClosure({ possibleTransitions.begin(), possibleTransitions.end() });

            // If this doesn't output state already exists as a generated state then push it into the toGenerateStates queue
            if (rawDFA.count(serialize(outputState)) == 0) 
                toGenerateStates.push(outputState);
            transitionFunctions.insert(make_pair(letter, serialize(outputState)));
        }
        // Insert this new generated set of transition functions into the rawDFA
        rawDFA[serializedState].insert(transitionFunctions.begin(), transitionFunctions.end());
        insertionOder.push_back(serializedState);
    }
    return rawDFAtoDFA(rawDFA, insertionOder);
}





// ___PRIVATE___

// Determines the e-closure for a specific set of states
vector<string> NFA::epsilonClosure(vector<string> states) {
    vector<string> candidateList(states.begin(), states.end());

    auto computeEpsilon = [&](string state) {
        // Get all the possible transitions for this state
        multimap<char, string> moveFunctions = this->transitionFunctions[state];
        // push in any epsilon transitions
        for (auto moveFunction: moveFunctions) {
            if (moveFunction.first == this->epsilon)
                candidateList.push_back(
                    moveFunction.second
                );
        }
    };
    for_each(states.begin(), states.end(), computeEpsilon);

    return candidateList;
}



// Converts a raw DFA tables into a plain DFA
DFA NFA::rawDFAtoDFA(map<string, map<char, string>> rawDFA, vector<string> insertionOrder) {
    map<string, string> stateIDtoState; // mapping of rawDFA state representations to real state representations
    DFA dfa(alphabet);

    // First pass to register all the states in the DFA
    for (string insertion: insertionOrder) {
        auto state = rawDFA[insertion];
        // Determine if the state that we are checking actually is an accepting state 
        auto couldBeAnAcceptingState = [&](string state){
            return insertion.find(state) != string::npos;
        };
        const bool isAnAcceptingState = any_of(acceptingStates.begin(), 
                                                acceptingStates.end(), 
                                                couldBeAnAcceptingState);
        stateIDtoState.insert(
            make_pair(insertion, dfa.addState(isAnAcceptingState)));
    }
    // Second pass to actually connect the nodes together through transition functions
    for (auto state: rawDFA) {
        for (auto transitionFunction: state.second) {
            dfa.constructTransition(stateIDtoState[state.first], 
                                    stateIDtoState[transitionFunction.second], 
                                    transitionFunction.first);
        }
    }

    return dfa;
}


// Specific to the DFA this is just a function that returns all the possible places a specific state can take us
vector<string> NFA::possibleDestinations(string state, char letter) {
    vector<string> candidates;
        
    for (pair<char, string> function: transitionFunctions[state]) {
        if (function.first == letter) candidates.push_back(function.second);
    }
        
    return candidates;
}



// Adds an NFA to this current NFA
std::tuple<std::string, 
            std::string, 
            std::map<std::string, std::string>> 
NFA::add(NFA with) {
    auto intial_terminating_state = *(this->acceptingStates.begin());
    auto terminating_state_with = *(with.acceptingStates.begin());

    // Due to the inefficencies in the NFA implementation 
    std::map<std::string, std::string> with_to_new;
    for (auto state: with.states) {
        bool is_accepting_state = acceptingStates.count(state) != 0;
        with_to_new.insert(std::make_pair(
            state, addState(is_accepting_state)
        ));
    }
    // Insert the transition functions of the "with" into this NFA
    for (auto state_functions: with.transitionFunctions) {
        for (auto function: state_functions.second) {
            constructTransition(
                with_to_new[state_functions.first], with_to_new[function.second], function.first
            );
        }
    }

    return std::make_tuple(intial_terminating_state, terminating_state_with, with_to_new);
}





// REGEX OPERATIONS BEGIN ===================
// Concats an NFA with another
void NFA::concat(NFA with) {
    // Determine the output of the summation
    std::string intial_terminating_state, terminating_state_with;
    std::map<std::string, std::string> with_to_new;
    std::tie(intial_terminating_state, terminating_state_with, with_to_new) = add(with);

    // Logically the generated NFAs only have a single accepting state, this means we can just pass epsilon transitions between the two
    constructTransition(intial_terminating_state, with_to_new["s-0"], NFA::epsilon);
    de_register_terminating_state(intial_terminating_state);
}

void NFA::kleene_star() {
    std::string intial_terminating_state = *(this->acceptingStates.begin());
    std::string new_initial_state = addState(false);
    std::string new_terminating_state = addState(true);

    // Main logic
    constructTransition(new_initial_state, initialState, NFA::epsilon);
    constructTransition(new_initial_state, new_terminating_state, NFA::epsilon);
    constructTransition(intial_terminating_state, initialState, NFA::epsilon);
    constructTransition(intial_terminating_state, new_terminating_state, NFA::epsilon);

    this->initialState = new_initial_state;
    de_register_terminating_state(intial_terminating_state);
}

// Unions this NFA with another NFA
void NFA::unionise(NFA with) {
    // Determine the output of the summation
    std::string intial_terminating_state, terminating_state_with;
    std::map<std::string, std::string> with_to_new;
    std::tie(intial_terminating_state, terminating_state_with, with_to_new) = add(with);

    // Deregister all terminating states
    std::string new_intial_state = addState(false);
    constructTransition(new_intial_state, initialState, NFA::epsilon);
    constructTransition(new_intial_state, with_to_new["s-0"], NFA::epsilon);
    // Constru the terminating state
    std::string new_terminating_state = addState(true);
    constructTransition(intial_terminating_state, new_terminating_state, NFA::epsilon);
    constructTransition(terminating_state_with, new_terminating_state, NFA::epsilon);
    // Kill off all the existing terminating states
    de_register_terminating_state(intial_terminating_state);
    de_register_terminating_state(terminating_state_with);

    initialState = new_intial_state;
}

// END ======================================