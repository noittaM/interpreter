#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <optional> 

#include "Tokenizer.hpp"

class Parser {
private:
    std::vector<Token> m_inTokens;
    size_t m_index{0};
    
    

    std::optional<Token> getCurrent () {
        if (m_index < m_inTokens.size())
            return m_inTokens[m_index];
        return {};
    }

    Token take () {
        return(m_inTokens[m_index++]);
    } 


public:
    
    Parser (const std::vector<Token>& tokens)
    :   m_inTokens{ tokens }
    {
    }
    
    std::string parse () {
        std::string output{};
        while (this->getCurrent().has_value()) {
            switch (this->getCurrent().value().type) {
            case TokenType::_return:
                output.append(this->take().value.value() + ' ');
                if (this->getCurrent().has_value() && (this->getCurrent().value().type == TokenType::int_lit || this->getCurrent().value().type == TokenType::expression)) {
                    output.append(this->take().value.value() + ' '); 
                    if (this->getCurrent().has_value() && this->getCurrent().value().type == TokenType::semi) {
                        output.append(this->take().value.value() + '\n'); 
                    } else {
                        std::cerr << output << "hi\n";
                        std::cerr << "Expected ';'.\n";
                        exit(EXIT_FAILURE);
                    }
                } else {
                    std::cerr << "Expected expression.\n";
                    exit(EXIT_FAILURE);
                }
                break;

            case TokenType::int_lit:
                std::cerr << "What do I do with int literal: " << this->getCurrent().value().value.value() << "????.\n";
                exit(EXIT_FAILURE);

            case TokenType::semi:
                std::cerr << "What do I do with semi: ';'????.\n";
                exit(EXIT_FAILURE);
            
            default:
                std::cerr << "No idea what this token is.\n";
                std::cerr << "Token value: '" << this->getCurrent().value().value.value() << "'.\n";
                std::cerr << "Token type: '" << this->getCurrent().value().type << "'.\n";
                exit(EXIT_FAILURE);
                break;
            }
        }
        return output;
    }
};
#endif // PARSER_HPP