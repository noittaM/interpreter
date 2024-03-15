#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include <unordered_map>

#include "Tokenizer.hpp"

class Parser {
private:
    enum class VarType {
        _int
    };

    struct Variable {
        VarType type;
        std::string value;
    };

    std::vector<Token> m_inTokens;
    size_t m_index{0};
    std::unordered_map<std::string, Variable> m_variables;

    std::optional<Token> getCurrent() {
        if (m_index < m_inTokens.size())
            return m_inTokens[m_index];
        return {};
    }

    Token take() {
        return (m_inTokens[m_index++]);
    }

    int getVariableValue() {
        const std::string identifier { getCurrent().value().value.value() };
        if (m_variables.contains(identifier)) {
            return std::stoi(m_variables[identifier].value);
        } else {
            std::cerr << "Unrecognized identifier: " << identifier <<'\n';
            exit(EXIT_FAILURE);
        }
    }

    int praseExpression() {
        int expression;
        if (!getCurrent().has_value()) {
            std::cerr << "Expected integer literal.\n";
            exit(EXIT_FAILURE);
        }
        if (getCurrent().value().type == TokenType::int_lit) {
            expression = std::stoi(take().value.value());
        } else if (getCurrent().value().type == TokenType::identifier) {
            //TODO: type checking
            expression = getVariableValue();
            take();
        } else {
            std::cerr << "expected expression\n";
            exit(EXIT_FAILURE);
        }
        
        while (getCurrent().has_value()) {
            if (getCurrent().value().type == TokenType::plus) {
                take();
                if (!getCurrent().has_value() ) {
                    std::cerr << "Expected integer expression.\n";
                    exit(EXIT_FAILURE);
                } 
                if (getCurrent().value().type == TokenType::int_lit){
                    expression += std::stoi(take().value.value());
                } else if (getCurrent().value().type == TokenType::identifier) {
                    //TODO: type checking
                    expression += getVariableValue();
                    take();
                }
            } else {
                return expression;
            }
        } 
        return expression;
    }

    

    void assignVariable (const std::string& identifier) {
        if (!getCurrent().has_value() || getCurrent().value().type != TokenType::equal) {
            std::cerr << "Expected '='.\n";
            exit(EXIT_FAILURE);
        }
        take();

        // this is where some type checking should be made
        // to determine type of variable but idc now.
        int varValue = praseExpression();
        if (!getCurrent().has_value() || getCurrent().value().type != TokenType::semi) {
            std::cerr << "Exprected ';'.\n";
            exit(EXIT_FAILURE);
        }  
        take();

        if (m_variables.contains(identifier)) {
            m_variables[identifier].value = std::to_string(varValue);
        } else {
            std::cerr << '\'' << identifier << "' was never declared\n";
            exit(EXIT_FAILURE);
        }
       
    }


    void addVariable (const std::string& identifier) {
        if (!getCurrent().has_value() || getCurrent().value().type != TokenType::equal) {
            std::cerr << "Expected '='.\n";
            exit(EXIT_FAILURE);
        } 
        take();
        // this is where some type checking should be made
        // to determine type of variable but idc now.
        int varValue = praseExpression();
        if (m_variables.contains(identifier)) {
            std::cerr << "Variable " << identifier << "already exists.\n";
            exit(EXIT_FAILURE);
        }
        if (!getCurrent().has_value() || getCurrent().value().type != TokenType::semi) {
            std::cerr << "Exprected ';'.\n";
            exit(EXIT_FAILURE);
        }
        take();
        m_variables.insert( {identifier, { .type = VarType::_int, .value = std::to_string(varValue) }});
    }
    

public:
    Parser(const std::vector<Token> &tokens)
        : m_inTokens{tokens}
    {
    }

    std::string parse() {
        std::string output{};
        while (getCurrent().has_value()) {
            if (getCurrent().value().type == TokenType::_return) {
                take();
                int expression;
                if (getCurrent().has_value() && getCurrent().value().type == TokenType::identifier) {
                    const std::string identifier = getCurrent().value().value.value();
                    if (m_variables.contains(identifier)) {
                        expression = std::stoi(m_variables[identifier].value);
                        take();
                    } else {
                        std::cerr << "Undefined Variable: \"" << identifier <<"\"\n";
                        exit(EXIT_FAILURE);
                    }
                } else if (getCurrent().has_value() && getCurrent().value().type == TokenType::int_lit) {
                    expression = praseExpression();

                }
                if (getCurrent().has_value() && getCurrent().value().type == TokenType::semi) {
                    take();
                    exit(expression); // TODO: ?????                    
                } else {
                    std::cerr << "Expected ';'.\n";
                    exit(EXIT_FAILURE);
                }
            } else if (getCurrent().value().type == TokenType::let) {
                take();
                if (!getCurrent().has_value() || getCurrent().value().type != TokenType::identifier) {
                    std::cerr << "Expected identifier\n";
                    exit(EXIT_FAILURE);
                }
                const std::string identifier = take().value.value();
                addVariable(identifier);                
            } else if(getCurrent().value().type == TokenType::identifier) {
                const std::string identifier = take().value.value(); 
                assignVariable(identifier);
            } else if (getCurrent().value().type == TokenType::int_lit) {
                std::cerr << "What do I do with int literal: " << getCurrent().value().value.value() << "????.\n";
                exit(EXIT_FAILURE);
            } else if (getCurrent().value().type == TokenType::semi) {
                std::cerr << "What do I do with semi: " << getCurrent().value().value.value() << "????.\n";
                exit(EXIT_FAILURE);
            } else {
                std::cerr << "No idea what this token is.\n";
                std::cerr << "Token value: '" << getCurrent().value().value.value() << "'.\n";
                std::cerr << "Token type: '" << getCurrent().value().type << "'.\n";
                exit(EXIT_FAILURE);
            }
        }
        return output;
    }
};
#endif // PARSER_HPP