#ifndef __REGEX_H__
#define __REGEX_H__

#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>
#include <string>
#include "Regex/tokens.h"
#include "Automata/nfa.h"


typedef std::vector<std::pair<char, Tokens>> expr_tokens;


class Regex {
    public:
        void parse(std::string expression) {
            auto tokenised_expression = this->tokenise(expression);
            NFA regex = this->parse(tokenised_expression);

            this->compiled_regex = regex.epsilonClosureConvert();
        }



        // Determines weather a regex matches a sentence
        bool matches(std::string sentence) {
            return compiled_regex.accepts(sentence);
        }


    private:
        // Tokenises an expression
        expr_tokens tokenise(std::string expression) {
            // Final output
            expr_tokens tokenised_expression;

            auto convert_symbol = [&](char c) {
                tokenised_expression.push_back(
                                            std::make_pair(c, Tokens::parse(c)));
            };
            std::for_each(expression.begin(), expression.end(), convert_symbol);
            return tokenised_expression;
        }


        // Determines if the parenthesis in the tokenised expression is balanced
        bool is_balanced(expr_tokens expression) {
            // From the front move to the most recently matched closing brace
            auto back_march = expression.end();
            auto exists_closing_brace = [&](expr_tokens::iterator openingBrace) {
                if (openingBrace->first != '(') return true;

                for (auto it = openingBrace; it != back_march; it++) {
                    if (it->first == ')') {
                        back_march = it + 1;
                        return true;
                    }
                }
            };

            return std::all_of(expression.begin(), expression.end(), exists_closing_brace);
        }


        // Cleans up and initializes an expression, function assumes the expression is balanced
        expr_tokens intialize(expr_tokens expression) {
            // First insert a terminating expression
            auto fin_expression = expression;
            fin_expression.insert(fin_expression.end(), {
                std::make_pair('.', Tokens::Token_Values::CONCATENATION),
                std::make_pair('\\', Tokens::Token_Values::NULL_SYMBOL)
            });


            // Iterate over the expression and whenever you encounter a uniary expression insert a set of parenthesis in front
            auto insert_paren = [&](expr_tokens::iterator charachter) {
                if (charachter->second != Tokens::Token_Values::KLEENE_STAR) return;
                else {
                    if ((charachter - 1)->second == Tokens::Token_Values::RIGHT_PARENTHESIS) return;

                    // This charachter has never been touched before so insert the ()
                    auto opening_iter = charachter - 1;
                    auto closing_iter = charachter;
                    // Insert the parentheses
                    fin_expression.insert(closing_iter, std::make_pair(')', Tokens::Token_Values::RIGHT_PARENTHESIS));
                    fin_expression.insert(opening_iter, std::make_pair('(', Tokens::Token_Values::LEFT_PARENTHESIS));
                }
            };
            std::for_each(fin_expression.begin() + 1, fin_expression.end(), insert_paren);

            // Insert concaenation operators in front of two connected symbols
            auto insert_concat = [&](expr_tokens::iterator charachter) {
                auto char_token = charachter->second;
                auto prev_token = (charachter - 1)->second;

                if (   char_token != Tokens::Token_Values::SYMBOL
                    || char_token != Tokens::Token_Values::LEFT_PARENTHESIS
                    || prev_token != Tokens::Token_Values::SYMBOL
                    || prev_token != Tokens::Token_Values::RIGHT_PARENTHESIS) return;

                fin_expression.insert((charachter - 1), std::make_pair(
                    '.', Tokens::Token_Values::CONCATENATION
                ));
            };
            std::for_each(fin_expression.begin() + 1, fin_expression.end(), insert_concat);


            return fin_expression;
        }

        // Computes the NFA from the conjoining of two different expressions
        NFA compute_nfa(NFA half_one, NFA half_two, Tokens op) {
            switch (op.value) {
            case Tokens::Token_Values::UNION:
                half_one.unionise(half_two);
                return half_one;
            case Tokens::Token_Values::CONCATENATION:
                half_one.concat(half_two);
                return half_two;
            case Tokens::Token_Values::KLEENE_STAR:
                half_one.kleene_star();
                return half_one;
            default:   
                return NFA();
            }
        }


        // Parse a given expression
        NFA parse(expr_tokens expression) {
            if (!is_balanced(expression)) return;
            auto working_expression = intialize(expression);


            // find the first instance of an operator
            auto is_operation = [&](expr_tokens::iterator charachter) {
                auto token = charachter->second;

                return token == Tokens::Token_Values::KLEENE_STAR
                    || token == Tokens::Token_Values::CONCATENATION
                    || token == Tokens::Token_Values::UNION;
            };
            auto first_operator = std::find_if(expression.begin(), expression.end(), is_operation);

            if (first_operator != expression.end()) {
                auto first_half  = expr_tokens(expression.begin(), first_operator);
                auto second_half = expr_tokens(first_operator + 1, expression.end());

                return compute_nfa(parse(first_half), parse(second_half), first_operator->second);
            }

            // Since this is a lonely expression :( just return a raw symbol NFA
            NFA raw_nfa;
            raw_nfa.addAlphabet(expression[0].first);
            std::string term_state = raw_nfa.addState(true);
            raw_nfa.constructTransition(raw_nfa.initialState, term_state, raw_nfa.epsilon);

            return raw_nfa;
        }



        DFA compiled_regex;

};


#endif