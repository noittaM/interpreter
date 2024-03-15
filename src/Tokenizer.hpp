#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <optional>

enum class TokenType {
    _return,
    identifier,
    let,
    int_lit,
    semi,
    plus,
    equal,
};


struct Token{
    const TokenType type;
    const std::optional<std::string> value;
};


class Tokenizer {
private:
    const std::string m_inString;
    size_t m_index{0};

   

    std::optional<char> getCurrent () const {
        if(m_index < m_inString.length())
            return m_inString[m_index];
        return {};
    }

    char take () {
        return(m_inString[m_index++]);
    } 

public:

    Tokenizer (const std::string& code)
    : m_inString{code}
    {
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens{};
        std::string buf{};

        while(getCurrent().has_value()) {
            if (std::isalpha(getCurrent().value())) {
                while (getCurrent().has_value() && std::isalnum(getCurrent().value())) {
                    buf.push_back(take());
                }

                if (buf == "return") {
                    tokens.push_back({ .type = TokenType::_return});
                    buf.clear();
                    continue;

                } else if (buf == "let") {
                    tokens.push_back({ .type = TokenType::let });
                    buf.clear();
                    continue;

                } else {
                    tokens.push_back({ .type = TokenType::identifier, .value = buf});
                    buf.clear();
                    continue; 

                }
            } else if (std::isdigit(getCurrent().value())) {
                while(std::isdigit(getCurrent().value())) {
                    buf.push_back(take());
                } 

                tokens.push_back({ .type = TokenType::int_lit, .value = buf });
                buf.clear();
                continue;

            } else if (getCurrent().value() == '+') {
                tokens.push_back({ .type = TokenType::plus});
                take();
                buf.clear();// TODO: why?
                continue;

            } else if (getCurrent().value() == '=') {
                tokens.push_back({ .type = TokenType::equal});
                take();
                buf.clear();
                continue;

            } else if (getCurrent().value() == ';') {
                tokens.push_back({ .type = TokenType::semi});
                take();
                buf.clear();
                continue;

            } else if(std::isspace(getCurrent().value())) {
                take();
                continue;
            
            } else {
                std::cerr << "Unrecognized symbol: '" << getCurrent().value() << "', dying rn...\n";
                exit(EXIT_FAILURE);
            }    
        }
        return tokens;
    }
};


std::ostream& operator<< (std::ostream& out, const TokenType value) {
    switch (value) {
        case TokenType::_return:
            return out << "_return";
        case TokenType::identifier:
            return out << "identifier";
        case TokenType::let:
            return out << "let";
        case TokenType::int_lit:
            return out << "int_lit";
        case TokenType::semi:
            return out << "semi";
        case TokenType::plus:
            return out << "plus";
        case TokenType::equal:
            return out << "equal";
        default:
            return out << "add a case in overload of operator<<\n";
    }
}


#endif //TOKENIZER_HPP