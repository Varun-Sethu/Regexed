#ifndef __PARSE_H__
#define __PARSE_H__


#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>
#include <string>
#include "Regex/tokens.h"

typedef std::vector<std::pair<char, Tokens>> expr_tokens;



expr_tokens tokenise(std::string expression) {
    // Final output
    expr_tokens tokenised_expression;

    auto convert_symbol = [&](char c) {
        tokenised_expression.push_back(std::make_pair(c, 
                                                    Tokens::parse(c)));
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
            if (it->second == ')') {
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

    return fin_expression;
}






#endif