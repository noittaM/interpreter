#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include <variant>
#include <algorithm>

#include "Tokenizer.hpp"


struct NodeTerm {
    std::variant < int, const std::string > term;
}; // TODO: 121212 see if string needs to be a pointer

struct BinaryOperator {
    TokenType _operator; // TODO: change TokenType to enum of operators?
};

struct NodeExpr;

struct NodeBinaryExpr {
    NodeExpr* leftOperand;
    BinaryOperator binaryOperator;
    NodeExpr* rightOperand;
};

struct NodeStatement;

struct NodeScope {
    std::vector < NodeStatement* > statements;
};

struct NodeExpr {
    std::variant < NodeTerm*, NodeBinaryExpr* > expression;
};

struct NodeDefineVar{
    const std::string& identifier;
    std::optional < NodeExpr* > value; // TODO: Types
};

struct NodeAssignVar {
    const std::string& identifier;
    NodeExpr* value;
};

struct NodeReturn {
    NodeExpr* value;
};

struct NodeStatement {
    std::variant < 
        NodeDefineVar*,
        NodeAssignVar*,
        NodeReturn*,
        NodeScope* 
    > statement; 
};

struct nodeProgram {
    std::vector < NodeStatement* > statements;
};


// TODO: remove vv
enum class VarType {
    _int
};

struct Variable {
    const std::string identifier;
    VarType type;
    std::string value;
};
// END TODO 

class Parser {
public:
    Parser(const std::vector<Token> &tokens)
        : m_inTokens{tokens}
    {
    }

    void parse() {
        #if 0
        while (getCurrent().has_value()) {
            if (getCurrent().value().type == TokenType::_return) {
                take();
                const int expression{ praseExpression() };
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
                if (!getCurrent().has_value()
                    || getCurrent().value().type != TokenType::identifier
                ) {
                    std::cerr << "Expected identifier\n";
                    exit(EXIT_FAILURE);
                }
                const std::string identifier = getCurrent().value().value.value();
                take();
                addVariable(identifier);                
            } else if(getCurrent().value().type == TokenType::identifier) {
                const std::string identifier = getCurrent().value().value.value();
                take();
                assignVariable(identifier);
            } else if (getCurrent().value().type == TokenType::int_lit) {
                std::cerr << "What do I do with int literal: " 
                << getCurrent().value().value.value() << "????.\n";
                exit(EXIT_FAILURE);
            } else if (getCurrent().value().type == TokenType::curly_open) {
                //parseScope();
                ;
            } else if (getCurrent().value().type == TokenType::semi) {
                std::cerr << "What do I do with semi: "
                << getCurrent().value().value.value() << "????.\n";
                exit(EXIT_FAILURE);
            } else {
                std::cerr << "No idea what this token is.\n" << 
                "Token value: '" << getCurrent().value().value.value() << "'.\n"
                << "Token type: '" << getCurrent().value().type << "'.\n";
                exit(EXIT_FAILURE);
            }
        }
        #endif 
        while (getCurrent().has_value()) {
            NodeStatement stmt { getNodeStatement() };
            prog.statements.push_back(&stmt);
        }
    }

private:
    std::optional<Token> getCurrent() {
        if (m_index < m_inTokens.size())
            return m_inTokens[m_index];
        return {};
    }

    void take() {
        m_index++;
    }

    std::optional<NodeTerm> getNodeTerm() {
        if (getCurrent().value().type == TokenType::int_lit) {
            std::optional<NodeTerm> node { std::stoi(getCurrent().value().value.value()) };
            return  node;
        } else if (getCurrent().value().type == TokenType::identifier)  {
            const std::string ident { getCurrent().value().value.value() };
            NodeTerm node { ident }; // TODO: 121212 i forgot why this is here
            return  node;
        } else {
            return {};
        }
    }

    std::optional<NodeBinaryExpr> getNodeBinaryExpr() {
        return {}; // TODO: 
    }

    std::optional<NodeExpr> getNodeExpression() {
        if (std::optional<NodeTerm> node { getNodeTerm() }) {
            return NodeExpr { &node.value() };
        } else if (std::optional<NodeBinaryExpr> node = getNodeBinaryExpr()) {
            return NodeExpr { &node.value() };
        } else {
            return {};
        }
    }

    std::optional<NodeReturn> getNodeReturn() {
        if (getCurrent().value().type != TokenType::_return) {
           return {};
        }
        take();
        std::optional<NodeExpr> expression { getNodeExpression() };

        if (!expression.has_value()) {
            std::cerr << "Expected expression.\n";
            exit(EXIT_FAILURE);
        }

        if (!getCurrent().has_value()
            || getCurrent().value().type != TokenType::semi) {
            std::cerr << "Expected ';'.\n";
            exit(EXIT_FAILURE);

        }
        take();
        return NodeReturn { &expression.value() };
    }

    std::optional<NodeDefineVar> getNodeDefineVariable() {
        if (getCurrent().value().type != TokenType::let) {
            return {};
        }
        take();
        if (!getCurrent().has_value()
        || getCurrent().value().type != TokenType::identifier) {
            std::cerr << "Expected identifier.\n";
            exit(EXIT_FAILURE);
        }
        const std::string identifier { getCurrent().value().value.value() };
        take();

        if (!getCurrent().has_value()) {
            std::cerr << "Expected '=' or ';'.\n";
            exit(EXIT_FAILURE);
        }

        if (getCurrent().value().type == TokenType::semi) {
            take();
            return NodeDefineVar { identifier, {} };
        } else if (getCurrent().value().type == TokenType::equal) {
            std::optional<NodeExpr> nodeExpr { getNodeExpression() };
            if (!nodeExpr.has_value()) {
                std::cerr << "Expected expression.\n";
                exit(EXIT_FAILURE);
            }
            if (!getCurrent().has_value() || getCurrent().value().type != TokenType::semi) {
                std::cerr << "Exprected ';'.\n";
                exit(EXIT_FAILURE);
            }
            take();
            return NodeDefineVar { identifier, &nodeExpr.value() };
        } else {
            std::cerr << "Expected '=' or ';'.\n";
            exit(EXIT_FAILURE);
        }
    }

    std::optional<NodeAssignVar> getNodeAssignVariable() {
        if (getCurrent().value().type != TokenType::identifier) {
            return {};
        }
        std::string identifier { getCurrent().value().value.value() };
        take();

        if(!getCurrent().has_value() 
            || getCurrent().value().type != TokenType::equal
        ) {
            std::cerr << "Expected '='.\n";
            exit(EXIT_FAILURE);
        }
        take();
        std::optional<NodeExpr> expr { getNodeExpression() };

        if (!expr.has_value()) {
            std::cerr << "Expected expression.\n";
            exit(EXIT_FAILURE);
        }
        if (getCurrent().value().type != TokenType::semi) {
            std::cerr << "Expected ';'.\n";
            exit(EXIT_FAILURE);
        }
        return NodeAssignVar { identifier, &expr.value()};
    }

    std::optional<NodeScope> getNodeScope() {
        if (getCurrent().value().type != TokenType::curly_open) {
            return {};
        }
        take();
        NodeScope scope;
        while (getCurrent().has_value() 
        && getCurrent().value().type != TokenType::curly_close
        ) {
            NodeStatement stmt { getNodeStatement() };
            scope.statements.push_back(&stmt);
        }
        return scope;
    }

    // TODO: check if getNodeStatme needs to be an optional or if it should fail
    NodeStatement getNodeStatement() {
        if (std::optional<NodeReturn> node { getNodeReturn() }) {
            return NodeStatement {&node.value()};
        } else if (std::optional<NodeDefineVar> node = getNodeDefineVariable()) {
            return NodeStatement {&node.value()};
        } else if (std::optional<NodeAssignVar> node = getNodeAssignVariable()) {
            return NodeStatement {&node.value()};
        } else if (std::optional<NodeScope> node = getNodeScope()) {
            return NodeStatement {&node.value()};
        } else {
            std::cerr << "Invalid expression.\n";
            exit(EXIT_FAILURE);
        }
    }

    // TODO: everything below here is trash and should probably be removed
    #if 0
    int getVariableValue() {
        const std::string identifier { getCurrent().value().value.value() };   
        const std::vector<Variable>::iterator it {
            std::find_if(
                m_variables.begin(),
                m_variables.end(),
                [&](const Variable& var){
                    return var.identifier == identifier;
                    })};
        if (it != m_variables.end()) {
            return std::stoi((*it).value);
        } else {
            std::cerr << "Undefined identifier: '" << identifier <<'\n';
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
            expression = std::stoi(getCurrent().value().value.value());
            take();
        } else if (getCurrent().value().type == TokenType::identifier) {
            //TODO: type checking
            expression = getVariableValue();
            take();
        } else {
            std::cerr << "expected expression.\n";
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
                    expression += std::stoi(getCurrent().value().value.value());
                    take();
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
        
        if (!getCurrent().has_value() ||
        getCurrent().value().type != TokenType::curly_open) {
            std::cerr << "Expected '{'.\n(how?)\n";
            exit(EXIT_FAILURE); 
        }
        return 0;
    }

    #endif
    std::vector<Token> m_inTokens;
    size_t m_index{};
    nodeProgram prog;
};

#endif // PARSER_HPP
