#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <optional>

enum class TokenType {
    _return,
    int_lit,
    expression,
    semi,
    plus,
};


struct Token{
    const TokenType type;
    const std::optional<std::string> value;
};


class Tokenizer {
private:
    const std::string m_inString;
    size_t m_index{0};

   

    std::optional<char> getCurrent () {
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

        while(this->getCurrent().has_value()) {
            if (std::isalpha(this->getCurrent().value())) {
                while (this->getCurrent().has_value() && std::isalnum(this->getCurrent().value())) {
                    buf.push_back(this->take());
                }
                if (buf == "return") {
                    tokens.push_back({ .type = TokenType::_return, .value = "return"});
                    buf.clear();
                    continue;
                } else {
                    std::cerr << "Unrecognized string: '" << buf << "',dying...\n";
                    exit(EXIT_FAILURE); 
                }           
            } else if (std::isdigit(this->getCurrent().value())) {
                while(std::isdigit(this->getCurrent().value())) {
                    buf.push_back(this->take());
                } 
                if (this->getCurrent().value() == '+') {
                    while (this->getCurrent().has_value() && this->getCurrent().value() == '+') {
                        buf.push_back('+');
                        this->take();
                        if (this->getCurrent().has_value() && std::isdigit(this->getCurrent().value())) {
                            while (this->getCurrent().has_value() && std::isdigit(this->getCurrent().value())) {
                                buf.push_back(this->take());
                            }
                        } else {
                            std::cerr << "Expected int.\n";
                            std::cerr << "dying...\n";
                            exit(EXIT_FAILURE);
                        }

                    }
                tokens.push_back({ .type = TokenType::expression, .value = buf });
                buf.clear();
                continue;
                } else {
                    tokens.push_back({.type = TokenType::int_lit, .value = buf});
                    buf.clear();
                    continue;
                }
            } else if (this->getCurrent().value() == ';') {
                tokens.push_back({ .type = TokenType::semi, .value = ";"});
                this->take();
                buf.clear();
                continue;
            } else if(std::isspace(this->getCurrent().value())) {
                this->take();
                continue;
            } else {
                std::cerr << "Unrecognized symbol: '" << this->getCurrent().value() << "', dying rn...\n";
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
        case TokenType::int_lit:
            return out << "int_lit";
        case TokenType::expression:
            return out << "expression";
        case TokenType::semi:
            return out << "semi";
        case TokenType::plus:
            return out << "plus";
        default:
            return out << "add a case in operator<<\n";
    }
}


#endif //TOKENIZER_HPP