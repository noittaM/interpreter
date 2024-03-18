#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <optional>

#include "Tokenizer.hpp"

class Parser {
public:
    Parser(const std::vector<Token> &tokens)
        : m_inTokens{tokens}
    {
    }

    void parse() {
        while (getCurrent().has_value()) {
            if (getCurrent().value().type == TokenType::_return) {
                take();
                int expression{ praseExpression() };
                if (getCurrent().has_value() &&
                    getCurrent().value().type == TokenType::semi) {
                    take();
                    exit(expression); // TODO: ?????                    
                } else {
                    std::cerr << "Expected ';'.\n";
                    exit(EXIT_FAILURE);
                }
            } else if (getCurrent().value().type == TokenType::let) {
                take();
                if (!getCurrent().has_value() ||
                    getCurrent().value().type != TokenType::identifier) {
                    std::cerr << "Expected identifier\n";
                    exit(EXIT_FAILURE);
                }
                const std::string identifier = take().value.value();
                addVariable(identifier);                
            } else if(getCurrent().value().type == TokenType::identifier) {
                const std::string identifier = take().value.value(); 
                assignVariable(identifier);
            } else if (getCurrent().value().type == TokenType::int_lit) {
                std::cerr << "What do I do with int literal: " 
                << getCurrent().value().value.value() << "????.\n";
                exit(EXIT_FAILURE);
            } else if (getCurrent().value().type == TokenType::semi) {
                std::cerr << "What do I do with semi: "
                << getCurrent().value().value.value() << "????.\n";
                exit(EXIT_FAILURE);
            } else {
                std::cerr << "No idea what this token is.\n" << 
                "Token value: '" << getCurrent().value().value.value() << "'.\n" <<
                "Token type: '" << getCurrent().value().type << "'.\n";
                exit(EXIT_FAILURE);
            }
        }
    }

private:
    enum class VarType {
        _int
    };

    struct Variable {
        const std::string identifier;
        VarType type;
        std::string value;
    };

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
        const std::vector<Variable>::iterator it {
            std::find_if(
                m_variables.begin(),
                m_variables.end(),
                [&](const Variable& var){ return var.identifier == identifier;}) };
        if (it != m_variables.end()) {
            return std::stoi((*it).value);
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
        const std::vector<Variable>::iterator it {
            std::find_if(
                m_variables.begin(),
                m_variables.end(),
                [&](const Variable& var){ return var.identifier == identifier;}) };
        if (it != m_variables.end()) {
            (*it).value = std::to_string(varValue);
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
        const std::vector<Variable>::iterator it {
            std::find_if(
                m_variables.begin(),
                m_variables.end(),
                [&](const Variable& var){ return var.identifier == identifier;}) };
        if (it != m_variables.end()) {
            std::cerr << "Variable " << identifier << "already exists.\n";
            exit(EXIT_FAILURE);
        }
        if (!getCurrent().has_value() || getCurrent().value().type != TokenType::semi) {
            std::cerr << "Exprected ';'.\n";
            exit(EXIT_FAILURE);
        }
        take();
        m_variables.push_back( { .identifier = identifier,  .type = VarType::_int, .value = std::to_string(varValue) });
    }
    
    int parseScope () {
        
        if (getCurrent().has_value() && getCurrent().value().type == TokenType::curly_close) {
            //delete scoped variables
            return 0; //????? optional maybe idk 
        }
    }

    std::vector<Token> m_inTokens;
    size_t m_index{0};
    std::vector<Variable> m_variables;
    std::vector<size_t> m_scopes;
};
#endif // PARSER_HPP