#ifndef __TOKENS_H__
#define __TOKENS_H__

// Tokens: *, (, ), |, ., '', a..b..c..
// Collection of tokens
class Tokens {
    public:
        enum class Token_Values {
            KLEENE_STAR,
            LEFT_PARENTHESIS,
            RIGHT_PARENTHESIS,
            UNION,
            CONCATENATION,
            SYMBOL,
            NULL_SYMBOL
        };
    public:
        Tokens(const Token_Values& v) : value{v} {}
        operator Tokens() const { return value; }
        Tokens& operator=(Token_Values v) { value = v; return *this;}
        bool operator==(const Token_Values v) const { return value == v; }
        bool operator!=(const Token_Values v) const { return value != v; }

        
        static Tokens parse(char c) {
            switch (c) {
                case '*': return Tokens(Token_Values::KLEENE_STAR);
                case '(': return Tokens(Token_Values::LEFT_PARENTHESIS);
                case ')': return Tokens(Token_Values::RIGHT_PARENTHESIS);
                case '|': return Tokens(Token_Values::UNION);
                case '.': return Tokens(Token_Values::CONCATENATION);
                default: return Tokens(Token_Values::SYMBOL);
            }
        }

    private:
        Token_Values value;
};


#endif