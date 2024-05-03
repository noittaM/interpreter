#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstdlib>
#include <ostream>
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include <variant>

#include "Tokenizer.hpp"

using int_literal = int;
using identifier = std::string;

struct NodeTerm {
    std::variant <int_literal, const identifier> term;
};

struct BinaryOperator {
    TokenType _operator; // TODO: change TokenType to enum of operators?
};

struct NodeExpr;

struct NodeBinaryExprAdd {
    NodeExpr* leftOperand;
    NodeExpr* rightOperand;
};

struct NodeBinaryExprSub {
    NodeExpr* leftOperand;
    NodeExpr* rightOperand;
};

struct NodeBinaryExprMult {
    NodeExpr* leftOperand;
    NodeExpr* rightOperand;
};

struct NodeBinaryExprDiv {
    NodeExpr* leftOperand;
    NodeExpr* rightOperand;
};

struct NodeBinaryExpr {
    std::variant <
        NodeBinaryExprAdd*,
        NodeBinaryExprSub*,
        NodeBinaryExprMult*,
        NodeBinaryExprDiv*
    > binaryExpr;
};

struct NodeStatement;

struct NodeScope {
    std::vector <NodeStatement *> statements;
};

struct NodeExpr {
    std::variant <NodeTerm *, NodeBinaryExpr *> expression;
};

struct NodeDefineVar{
    const std::string& identifier;
    std::optional <NodeExpr *> value; // TODO: Types
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

struct NodeProgram {
    std::vector <NodeStatement *> statements;
};

class Parser {
public:
    explicit Parser(const std::vector<Token> &tokens)
        : m_inTokens{tokens}
    {
    }

    // TODO: copy constructor and assignment
    // ~Parser() {
    //     for (const auto& stmt : m_prog.statements) {
    //         delete stmt;
    //     }
    //     // m_prog.statements.clear();
    // }

    NodeProgram parse() {
        while (std::optional<NodeStatement *> stmt { getNodeStatement() }) {
            // std::cout << stmt->statement.index() << std::endl;
            m_prog.statements.push_back(stmt.value());
        }
        return m_prog; 
    }

private:
    std::vector<Token> m_inTokens;
    size_t m_index{};
    NodeProgram m_prog;

private:
    std::optional<Token> peekToken(size_t offset = 0) {
        if (m_index + offset < m_inTokens.size())
            return m_inTokens[m_index + offset];
        return {};
    }

    void consumeToken() {
        std::cout << "consuming token" << std::endl;
        m_index++;
    }

    std::optional<NodeStatement *> getNodeStatement() {
        NodeStatement* stmt { new NodeStatement };

        if (std::optional<NodeDefineVar *> node = getNodeDefineVar()) {
            stmt->statement = node.value();
        } else if (std::optional<NodeAssignVar *> node = getNodeAssignVar()) {
            stmt->statement = node.value();
        } else if (std::optional<NodeReturn *> node = getNodeReturn()) {
            stmt->statement = node.value();
        } else {
            return {};
        }

        return stmt;
    }

    std::optional<NodeDefineVar *> getNodeDefineVar() {
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::let) {
            return {};
        }
        std::cout << "first consume 'let'" << std::endl;
        consumeToken();

        // expect identifier after 'let'
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::identifier) {
            std::cerr << "Expected identifier after let\n";
            exit(EXIT_FAILURE);
        }
        const std::string ident { peekToken().value().value.value() };
        std::cout << "second consume 'ident'" << std::endl;
        consumeToken();

        // valid variable declaration with no value
        if (peekToken().has_value()
            && peekToken().value().type == TokenType::semi) {
            consumeToken();
            NodeDefineVar* node { new NodeDefineVar {
                .identifier { ident }, // NOTE: copying strings around
                .value { {} }
            }};
            return node;
        }

        // expect '=' after identifier
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::equal) {
            std::cerr << "Expected '=' in identifier definition\n";
            exit(EXIT_FAILURE);
        }
        std::cout << "third consume, '='" << std::endl;
        consumeToken();

        // expect expression after '='
        std::optional<NodeExpr *> expr { getNodeExpr() };
        if (!expr.has_value()) {
            std::cerr << "Expected expression\n";
            exit(EXIT_FAILURE);
        }

        // expect ';'
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::semi) {
            std::cerr << "Expected ';' after variable defnition\n";
            exit(EXIT_FAILURE);
        }

        consumeToken();

        // NOTE: copying strings but whatever
        NodeDefineVar* node { new NodeDefineVar { ident, expr.value() } };
        return node;
    }

    std::optional<NodeAssignVar *> getNodeAssignVar() {
        if (!peekToken().has_value()) {
            return {};
        }

        if (peekToken().value().type != TokenType::identifier
            || peekToken(1).has_value()
            || peekToken(1).value().type == TokenType::equal
        ) {
            return {};
        }
        const std::string ident { peekToken().value().value.value() };
        consumeToken(); // token identifier
        consumeToken(); // token equal

        // expect expression after '='
        std::optional<NodeExpr *> expr { getNodeExpr() };
        if (!expr.has_value()) {
            std::cerr << "Expected expression\n";
            exit(EXIT_FAILURE);
        }

        // expect ';'
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::semi) {
            std::cerr << "Expected ';' in variable assignment\n";
            exit(EXIT_FAILURE);
        }
        consumeToken();

        // NOTE: copying strings but whatever
        NodeAssignVar* node { new NodeAssignVar { ident, expr.value() } };
        return node;
    }

    std::optional<NodeReturn *> getNodeReturn() {
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::_return) {
            return {};
        }
        consumeToken();

        // expect expression
        std::optional<NodeExpr *> expr { getNodeExpr() };
        if (!expr.has_value()) {
            std::cerr << "Expected expression after 'return'\n";
            exit(EXIT_FAILURE);
        }

        // expect ';'
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::semi) {
            std::cerr << "Expected ';' after return expression\n";
            exit(EXIT_FAILURE);
        }
        consumeToken();

        NodeReturn* node { new NodeReturn { expr.value() } };
        return node;
    }

    std::optional<NodeExpr *> getNodeExpr() {
        // term
        // binary expression
        NodeExpr* expr { new NodeExpr };
        // define var
        // assign var
        // return
        if (std::optional<NodeTerm *> node = getNodeTerm()) {
            expr->expression = node.value();
        } else if (std::optional<NodeBinaryExpr *> node = getNodeBinaryExpr()) {
            expr->expression = node.value();
        } else {
            return {};
        }
        return expr;
    }

    std::optional<NodeTerm *> getNodeTerm() {
        if (!peekToken().has_value()) {
            return {};
        }

        NodeTerm* term { new NodeTerm };

        if (peekToken().value().type == TokenType::identifier
            && !isOperator(peekToken(1))) {
            // NOTE: copying strings around again
            term->term.emplace<const identifier>(peekToken().value().value.value());
            consumeToken();
            return term;
        } else if (peekToken().value().type == TokenType::int_lit
                    && !isOperator(peekToken(1))) {
            int valueInt { std::stoi(peekToken().value().value.value()) };
            term->term.emplace<int_literal>(valueInt);
            consumeToken();
            return term;
        } else {
            return {};
        }
    }

    bool isOperator(std::optional<Token> token) {
        if (!token.has_value()) {
            return false;
        }

        if (token.value().type == TokenType::plus
            || token.value().type == TokenType::dash
            || token.value().type == TokenType::asterisk
            || token.value().type == TokenType::forward_slash
        ) {
            return true;
        }
        return false;
    }

    bool isTerm(std::optional<Token> token) {
        if (!token.has_value()) {
            return false;
        }

        if (token.value().type == TokenType::identifier
            || token.value().type == TokenType::int_lit
        ) {
            return true;
        }
        return false;
    }
    std::optional<NodeBinaryExpr *> getNodeBinaryExpr() {
        // TODO: make is so a binary expression can have multiple operators
        /*
        example:
        5 + 5 - 3 + 9
        vvvvv
        binaryAdd(5, 5) - 3 + 9
                    vvvvvv
        binarySub(binaryAdd(5, 5), 3) + 9
                                vvvvvvvvv
        binaryAdd(binarySub(binaryAdd(5, 5), 3), 9)
        */


        //*check if there is a term
        // if yes then make sure there is an operator
        // if either is not found, return {}
        // if both found then expect a right term
        // if not found explode
        // if found make a biexper depending on the operator
        // repeat *
        NodeExpr* leftOperand { new NodeExpr };
        NodeExpr* rightOperand { new NodeExpr };
        NodeBinaryExpr* binaryExpr { new NodeBinaryExpr };

        bool foundExpr = false;

        if (!isTerm(peekToken()) || !isOperator(peekToken(1))) {
            return {};
        }

        // expect a right operand since we found a left operand and operator
        if (!isTerm(peekToken(2))) {
            std::cerr << "Expected right operand\n";
        }

        leftOperand->expression.emplace<NodeTerm *>( new NodeTerm {
            .term { peekToken().value().value.value() }
        });

        TokenType _operator { peekToken(1).value().type };

        rightOperand->expression.emplace<NodeTerm *>(new NodeTerm {
            .term { peekToken(2).value().value.value() }
        });

        consumeToken(); // left operand
        consumeToken(); // operator
        consumeToken(); // right operand


        if (_operator == TokenType::plus) {
            NodeBinaryExprAdd* node {
                new NodeBinaryExprAdd { leftOperand, rightOperand }
            };
            binaryExpr->binaryExpr.emplace<NodeBinaryExprAdd *>(node);
            foundExpr = true;

        } else if (_operator == TokenType::dash) {
            NodeBinaryExprSub* node {
                new NodeBinaryExprSub { leftOperand, rightOperand }
            };
            binaryExpr->binaryExpr.emplace<NodeBinaryExprSub *>(node);
            foundExpr = true;

        } else if (_operator == TokenType::asterisk) {
            NodeBinaryExprMult* node {
                new NodeBinaryExprMult { leftOperand, rightOperand }
            };
            binaryExpr->binaryExpr.emplace<NodeBinaryExprMult *>(node);
            foundExpr = true;

        } else if (_operator == TokenType::forward_slash) {
            NodeBinaryExprDiv* node {
                new NodeBinaryExprDiv { leftOperand, rightOperand }
            };
            binaryExpr->binaryExpr.emplace<NodeBinaryExprDiv *>(node);
            foundExpr = true;

        } else {
            std::cerr << "Invalid operator.\n";
            exit(EXIT_FAILURE);
        }

        if (foundExpr) {
            return binaryExpr;
        }
        return {};

        #if 0
        std::optional<NodeTerm> leftTerm { getNodeTerm().value() };
        if (!leftTerm.has_value()) {
            return {};
        }

        // not calling take() yet as there is still a chance of returning nothing
        if (!peekToken().has_value()) {
            return {};
        }

        NodeBinaryExpr binaryExpr;
        NodeExpr leftOperand { &leftTerm.value() };
        NodeTerm rightTerm {0};
        NodeExpr rightOperand { &rightTerm };
        NodeBinaryExprAdd node { &leftOperand, &rightOperand };
        binaryExpr.binaryExpr.emplace<NodeBinaryExprAdd*>(&node);

        bool foundExpression = false;
        while (true) {
            std::optional<TokenType> _operator { peekToken(1).value().type };
            if (!_operator.has_value() || !isOperator(_operator.value())) {
                break;
            }
            std::optional<NodeTerm> rightTerm { getNodeTerm(2) };
            if (!rightTerm.has_value()) {
                break;
            }
            take();
            take();
            // don't take a third time
            NodeExpr leftOperand { &binaryExpr }; // look at example
            NodeExpr rightOperand { &rightTerm.value() }; //get next right operand
            if (_operator == TokenType::plus) {
                NodeBinaryExprAdd node { &leftOperand, &rightOperand };
                binaryExpr.binaryExpr.emplace<NodeBinaryExprAdd*>(&node);
                foundExpression = true;
            } else if (_operator == TokenType::dash) {
                NodeBinaryExprSub node { &leftOperand, &rightOperand };
                binaryExpr.binaryExpr.emplace<NodeBinaryExprSub*>(&node);
                foundExpression = true;
            } else if (_operator == TokenType::asterisk) {
                NodeBinaryExprMult node { &leftOperand, &rightOperand };
                binaryExpr.binaryExpr.emplace<NodeBinaryExprMult*>(&node);
                foundExpression = true;
            } else if (_operator == TokenType::forward_slash) {
                NodeBinaryExprDiv node { &leftOperand, &rightOperand };
                binaryExpr.binaryExpr.emplace<NodeBinaryExprDiv*>(&node);
                foundExpression = true;
            } else {
                std::cerr << "Invalid operator.\n";
                exit(EXIT_FAILURE);
            }
        }
        if (foundExpression) {
            take(); // take() to because we wored with an extra token
            return binaryExpr;
        }
        // if there was no expression (out biExrp = binaryAdd(5, 0))
        // return nothing and don't take()
        return {};
        #endif
    }





    #if 0
    std::optional<NodeTerm> tryGetNode() {
        NodeTerm node;
        if (!peekToken().has_value()) {
            return {};
        }
        if (peekToken().value().type == TokenType::int_lit) {
            node.term.emplace<int>(std::stoi(peekToken().value().value.value()));
        } else if (peekToken().value().type == TokenType::identifier)  {
            const std::string ident { peekToken().value().value.value() };
            node.term.emplace<const std::string*>(&ident);
        } else {
            return {};
        }

        if (peekToken(1).has_value() && isOperator(peekToken(1).value().type)) {
            return {};
        }
        take();
        return node;
    }


    std::optional<NodeTerm> getNodeTerm(size_t offset = 0) {
        if (!peekToken(offset).has_value()) {
            return {};
        }
        if (peekToken(offset).value().type == TokenType::int_lit) {
            NodeTerm node {std::stoi(peekToken().value().value.value())};
            take();
            return node;
        } else if (peekToken(offset).value().type == TokenType::identifier)  {
            const std::string& ident { peekToken().value().value.value() };
            NodeTerm node { &ident };
            take();
            return node;
        } else {
            return {};
        }
    }


    bool isOperator(TokenType type) {
        if (type == TokenType::plus
        || type == TokenType::dash
        || type == TokenType::asterisk
        || type == TokenType::forward_slash
        ) {
            return true;
        }
        return false;
    }

    std::optional<NodeBinaryExpr> getNodeBinaryExpr() {
        /*
        example:
        5 + 5 - 3 + 9
        vv
        binaryAdd(5, 0) + 5 - 3 + 9
                    vvvvvv
        binaryAdd(binaryAdd(5, 0), 5) - 3 + 9
                                vvvvvvvvv
        binarySub(binaryAdd(binaryAdd(5, 0), 5), 3) + 9
                                                vvvvvvv
        binaryAdd(binarySub(binaryAdd(binaryAdd(5, 0), 5), 3), 9)
        */
        std::optional<NodeTerm> leftTerm { getNodeTerm().value() };
        if (!leftTerm.has_value()) {
            return {};
        }

        // not calling take() yet as there is still a chance of returning nothing
        if (!peekToken().has_value()) {
            return {};
        }

        NodeBinaryExpr binaryExpr;
        NodeExpr leftOperand { &leftTerm.value() };
        NodeTerm rightTerm {0};
        NodeExpr rightOperand { &rightTerm };
        NodeBinaryExprAdd node { &leftOperand, &rightOperand };
        binaryExpr.binaryExpr.emplace<NodeBinaryExprAdd*>(&node);

        bool foundExpression = false;
        while (true) {
            std::optional<TokenType> _operator { peekToken(1).value().type };
            if (!_operator.has_value() || !isOperator(_operator.value())) {
                break;
            }
            std::optional<NodeTerm> rightTerm { getNodeTerm(2) };
            if (!rightTerm.has_value()) {
                break;
            }
            take();
            take();
            // don't take a third time
            NodeExpr leftOperand { &binaryExpr }; // look at example
            NodeExpr rightOperand { &rightTerm.value() }; //get next right operand
            if (_operator == TokenType::plus) {
                NodeBinaryExprAdd node { &leftOperand, &rightOperand };
                binaryExpr.binaryExpr.emplace<NodeBinaryExprAdd*>(&node);
                foundExpression = true;
            } else if (_operator == TokenType::dash) {
                NodeBinaryExprSub node { &leftOperand, &rightOperand };
                binaryExpr.binaryExpr.emplace<NodeBinaryExprSub*>(&node);
                foundExpression = true;
            } else if (_operator == TokenType::asterisk) {
                NodeBinaryExprMult node { &leftOperand, &rightOperand };
                binaryExpr.binaryExpr.emplace<NodeBinaryExprMult*>(&node);
                foundExpression = true;
            } else if (_operator == TokenType::forward_slash) {
                NodeBinaryExprDiv node { &leftOperand, &rightOperand };
                binaryExpr.binaryExpr.emplace<NodeBinaryExprDiv*>(&node);
                foundExpression = true;
            } else {
                std::cerr << "Invalid operator.\n";
                exit(EXIT_FAILURE);
            }
        }
        if (foundExpression) {
            take(); // take() to because we wored with an extra token
            return binaryExpr;
        }
        // if there was no expression (out biExrp = binaryAdd(5, 0))
        // return nothing and don't take()
        return {};
    }


    std::optional<NodeExpr> getNodeExpression() {
        if (!peekToken().has_value()) {
            return {};
        }
        if (std::optional<NodeTerm> node { tryGetNodeTerm() }) {
            return NodeExpr { &node.value() };
        } else if (std::optional<NodeBinaryExpr> node { getNodeBinaryExpr() }) {
            return NodeExpr { &node.value() };
        } else {
            return {};
        }
    }

    std::optional<NodeReturn> getNodeReturn() {
        if (!peekToken().has_value()) {
            return {};
        }
        if (peekToken().value().type != TokenType::_return) {
           return {};
        }
        take();

        if (!peekToken().has_value()) {
            std::cerr << "Expected expression.\n";
            exit(EXIT_FAILURE);
        }

        std::optional<NodeExpr> expression { getNodeExpression() };

        if (!expression.has_value()) {
            std::cerr << "Expected expression.\n";
            exit(EXIT_FAILURE);
        }

        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::semi
            ) {
            std::cerr << "Expected ';'.\n";
            exit(EXIT_FAILURE);

        }
        take();
        return NodeReturn { &expression.value() };
    }

    std::optional<NodeDefineVar> getNodeDefineVariable() {
        if (!peekToken().has_value()) {
            return {};
        }
        if (peekToken().value().type != TokenType::let) {
            return {};
        }
        take();
        
        if (!peekToken().has_value()
        || peekToken().value().type != TokenType::identifier
        ) {
            std::cerr << "Expected identifier.\n";
            exit(EXIT_FAILURE);
        }
        const std::string identifier { peekToken().value().value.value() };
        take();

        if (!peekToken().has_value()) {
            std::cerr << "Expected '=' or ';'.\n";
            exit(EXIT_FAILURE);
        }

        if (peekToken().value().type == TokenType::semi) {
            take();
            return NodeDefineVar { identifier, {} };
        } else if (peekToken().value().type == TokenType::equal) {
            take();
            std::optional<NodeExpr> nodeExpr { getNodeExpression() };
            if (!nodeExpr.has_value()) {
                std::cerr << "Expected expression.\n";
                exit(EXIT_FAILURE);
            }

            if (!peekToken().has_value() 
            || peekToken().value().type != TokenType::semi
            ) {
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
        if (!peekToken().has_value()
        || peekToken().value().type != TokenType::identifier
        ) {
            return {};
        }
        std::string identifier { peekToken().value().value.value() };
        take();

        if(!peekToken().has_value() 
        || peekToken().value().type != TokenType::equal
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
        if (peekToken().value().type != TokenType::semi) {
            std::cerr << "Expected ';'.\n";
            exit(EXIT_FAILURE);
        }
        return NodeAssignVar { identifier, &expr.value() };
    }

    std::optional<NodeScope> getNodeScope() {
        if (!peekToken().has_value()
        || peekToken().value().type != TokenType::curly_open) {
            return {};
        }
        take();
        NodeScope scope;
        while (peekToken().has_value() 
        && peekToken().value().type != TokenType::curly_close
        ) {
            NodeStatement stmt { getNodeStatement() };
            scope.statements.push_back(&stmt);
        }
        return scope;
    }

    // TODO: check if getNodeStatme needs to be an optional or if it should fail
    NodeStatement getNodeStatement() {
        if (!peekToken().has_value()) {
            return {};
        }

        if (std::optional<NodeReturn> node { getNodeReturn() }) {
            return NodeStatement { &node.value() };

        } else if (std::optional<NodeDefineVar> node { getNodeDefineVariable() }) {
            return NodeStatement { &node.value() };

        } else if (std::optional<NodeAssignVar> node { getNodeAssignVariable() }) {
            return NodeStatement { &node.value() };

        } else if (std::optional<NodeScope> node { getNodeScope() }) {
            return NodeStatement { &node.value()};

        } else {
            std::cerr << "Invalid expression.\n";
            exit(EXIT_FAILURE);
        }
    }

    // TODO: everything here is trash and should probably be removed
    #if 0
    int getVariableValue() {
        const std::string identifier { peekToken().value().value.value() };   
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
        if (!peekToken().has_value()) {
            std::cerr << "Expected integer literal.\n";
            exit(EXIT_FAILURE);
        }

        if (peekToken().value().type == TokenType::int_lit) {
            expression = std::stoi(peekToken().value().value.value());
            take();
        } else if (peekToken().value().type == TokenType::identifier) {
            //TODO: type checking
            expression = getVariableValue();
            take();
        } else {
            std::cerr << "expected expression.\n";
            exit(EXIT_FAILURE);
        }
        
        while (peekToken().has_value()) {
            if (peekToken().value().type == TokenType::plus) {
                take();
                if (!peekToken().has_value() ) {
                    std::cerr << "Expected integer expression.\n";
                    exit(EXIT_FAILURE);
                } 

                if (peekToken().value().type == TokenType::int_lit){
                    expression += std::stoi(peekToken().value().value.value());
                    take();
                } else if (peekToken().value().type == TokenType::identifier) {
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
        if (!peekToken().has_value() || peekToken().value().type != TokenType::equal) {
            std::cerr << "Expected '='.\n";
            exit(EXIT_FAILURE);
        }
        take();

        // this is where some type checking should be made
        // to determine type of variable but idc now.
        int varValue = praseExpression();
        if (!peekToken().has_value() || peekToken().value().type != TokenType::semi) {
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
        if (!peekToken().has_value() || peekToken().value().type != TokenType::equal) {
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
        if (!peekToken().has_value() || peekToken().value().type != TokenType::semi) {
            std::cerr << "Exprected ';'.\n";
            exit(EXIT_FAILURE);
        }
        take();
        m_variables.push_back( { .identifier = identifier,  .type = VarType::_int, .value = std::to_string(varValue) });
    }

    int parseScope () {

        if (!peekToken().has_value() ||
        peekToken().value().type != TokenType::curly_open) {
            std::cerr << "Expected '{'.\n(how?)\n";
            exit(EXIT_FAILURE); 
        }
        return 0;
    }
    #endif

    #endif

};

inline std::ostream& operator<< (std::ostream& out, NodeStatement* node) {
    if (std::holds_alternative<NodeDefineVar*>(node->statement)) {
        return out << "NodeDefineVar";
    } else if (std::holds_alternative<NodeAssignVar*>(node->statement)) {
        return out << "NodeAssignVar";
    } else if (std::holds_alternative<NodeReturn*>(node->statement)) {
        return out << "NodeReturn";
    } else if (std::holds_alternative<NodeScope*>(node->statement)) {
        return out << "NodeScope";
    } else {
        out << node->statement.index();
        return out << "add a case for Statement in overload of operator<<";
    }
}
#endif // PARSER_HPP
