#ifndef PARSER_HPP
#define PARSER_HPP

#include <cassert>
#include <cstdlib>
#include <ostream>
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include <variant>

#include "Tokenizer.hpp"

using int_literal = int;
using ident = const std::string;

struct NodeTerm {
    std::variant <int_literal, ident> term;
};

struct BinaryOperator {
    TokenType _operator; // TODO: change TokenType to enum of operators?
};

struct NodeBinaryExpr {
    std::vector<TokenType> operators;
    std::vector<NodeTerm *> operands;
};

struct NodeExpr {
    std::variant <NodeTerm *, NodeBinaryExpr *> expression;
};

struct NodeStmtDefineVar{
    const std::string identifier;
    std::optional <NodeExpr *> value; // TODO: Types
};

struct NodeStmtAssignVar {
    const std::string identifier;
    NodeExpr* value;
};

struct NodeStmtReturn {
    NodeExpr* value;
};

struct NodeStmtScope;

struct NodeStatement {
    std::variant <
        NodeStmtDefineVar*,
        NodeStmtAssignVar*,
        NodeStmtReturn*,
        NodeStmtScope*
    > statement;
};

struct NodeStmtScope {
    std::vector <NodeStatement *> statements;
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

    // FIXME: need a destructor
    // TODO: copy constructor and assignment
    // ~Parser() {
    //     for (const auto& stmt : m_prog.statements) {
    //         delete stmt;
    //     }
    //     // m_prog.statements.clear();
    // }

    NodeProgram parse() {
        while (std::optional<NodeStatement *> stmt { getNodeStatement() }) {
            m_prog.statements.push_back(stmt.value());
        }
        return m_prog;
    }

private:
    std::optional<Token> peekToken(size_t offset = 0) {
        if (m_index + offset < m_inTokens.size())
            return m_inTokens[m_index + offset];
        return {};
    }

    void consumeToken() {
        m_index++;
    }

    std::optional<NodeStatement *> getNodeStatement() {
        NodeStatement* stmt;

        if (std::optional<NodeStmtDefineVar *> node = getNodeDefineVar()) {
            stmt = new NodeStatement;
            stmt->statement = node.value();
            return stmt;
        } else if (std::optional<NodeStmtAssignVar *> node = getNodeAssignVar()) {
            stmt = new NodeStatement;
            stmt->statement = node.value();
            return stmt;
        } else if (std::optional<NodeStmtReturn *> node = getNodeReturn()) {
            stmt = new NodeStatement;
            stmt->statement = node.value();
            return stmt;
        } else if (std::optional<NodeStmtScope *> node = getNodeScope()) {
            stmt = new NodeStatement;
            stmt->statement = node.value();
            return stmt;
        } else {
            return {};
        }
    }

    std::optional<NodeStmtDefineVar *> getNodeDefineVar() {
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::let) {
            return {};
        }
        consumeToken();

        // expect identifier after 'let'
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::identifier) {
            std::cerr << "Expected identifier after let\n";
            exit(EXIT_FAILURE);
        }
        const std::string ident { peekToken().value().value.value() };
        consumeToken();

        // valid variable declaration with no value
        if (peekToken().has_value()
            && peekToken().value().type == TokenType::semi) {
            consumeToken();
            NodeStmtDefineVar* node { new NodeStmtDefineVar {
                .identifier { ident }, // NOTE: copying strings around
                .value { }
            }};
            return node;
        }

        // expect '=' after identifier
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::equal) {
            std::cerr << "Expected '=' in identifier definition\n";
            exit(EXIT_FAILURE);
        }
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
        NodeStmtDefineVar* node { new NodeStmtDefineVar {
            .identifier { ident },
            .value { expr.value() }
        }};
        return node;
    }

    std::optional<NodeStmtAssignVar *> getNodeAssignVar() {
        if (!peekToken().has_value()) {
            return {};
        }

        if (peekToken().value().type != TokenType::identifier
            || !peekToken(1).has_value()
            || peekToken(1).value().type != TokenType::equal
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
        NodeStmtAssignVar* node { new NodeStmtAssignVar {
           .identifier { ident },
           .value { expr.value() }
        }};
        return node;
    }

    std::optional<NodeStmtReturn *> getNodeReturn() {
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::_return
        ) {
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
            || peekToken().value().type != TokenType::semi
        ) {
            std::cerr << "Expected ';' after return expression\n";
            exit(EXIT_FAILURE);
        }
        consumeToken();

        NodeStmtReturn* node { new NodeStmtReturn {
            .value {expr.value() }
        }};
        return node;
    }

    std::optional<NodeStmtScope *> getNodeScope() {
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::curly_open
        ) {
            return {};
        }
        consumeToken();

        NodeStmtScope* scope { new NodeStmtScope };
        while (std::optional<NodeStatement *> stmt { getNodeStatement() }) {
            scope->statements.push_back(stmt.value());
        }

        if (peekToken().has_value()
            && peekToken().value().type == TokenType::curly_close
        ) {
            consumeToken();
            return scope;
        } else {
            std::cerr << "expected '}'\n";
            exit(EXIT_FAILURE);
        }
    }

    std::optional<NodeExpr *> getNodeExpr() {
        NodeExpr* expr;
        if (std::optional<NodeTerm *> node = getNodeTerm()) {
            expr = new NodeExpr;
            expr->expression = node.value();
            return expr;
        } else if (std::optional<NodeBinaryExpr *> node = getNodeBinaryExpr()) {
            expr = new NodeExpr;
            expr->expression = node.value();
            return expr;
        } else {
            return {};
        }
    }

    std::optional<NodeTerm *> getNodeTerm() {
        if (!peekToken().has_value()) {
            return {};
        }

        NodeTerm* term;

        if (peekToken().value().type == TokenType::identifier
            && !isOperator(peekToken(1))) {
            // NOTE: copying strings around again

            term = new NodeTerm;
            term->term.emplace<ident>(peekToken().value().value.value());
            consumeToken();
            return term;
        } else if (peekToken().value().type == TokenType::int_lit
                    && !isOperator(peekToken(1))) {
            int valueInt { std::stoi(peekToken().value().value.value()) };

            term = new NodeTerm;
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
        if (!isOperator(peekToken(1))) {
            return {};
        }

        NodeBinaryExpr* expr;

        if (peekToken().value().type == TokenType::identifier) {
            assert((peekToken().value().value.has_value() == true) &&
                   "parser error: got identifier with no value\n");

            expr = new NodeBinaryExpr;
            NodeTerm* term { new NodeTerm {
               .term { peekToken().value().value.value() }
            }};

            expr->operands.push_back(term);

            consumeToken();
        } else if (peekToken().value().type == TokenType::int_lit) {
            assert (peekToken().value().value.has_value() == true &&
                    "parser error: got int literal with no value\n");

            expr = new NodeBinaryExpr;
            NodeTerm* term { new NodeTerm {
               .term { std::stoi(peekToken().value().value.value()) }
            }};

            expr->operands.push_back(term);

            consumeToken();
        } else {
            return {};
        }

        while (true) {
            if (!isOperator(peekToken())) {
                break;
            }

            expr->operators.push_back(peekToken().value().type);
            consumeToken();

            if (peekToken().value().type == TokenType::identifier) {
                if (!peekToken().value().value.has_value()) {
                    std::cerr << "parser error: got identifier with no value\n";
                }

                NodeTerm* term { new NodeTerm {
                    .term { peekToken().value().value.value() }
                }};
                expr->operands.push_back(term);

                consumeToken();

            } else if (peekToken().value().type == TokenType::int_lit) {
                if (!peekToken().value().value.has_value()) {
                    std::cerr << "parser error: got int literal with no value\n";
                }

                NodeTerm* term { new NodeTerm {
                    .term { std::stoi(peekToken().value().value.value()) }
                }};
                expr->operands.push_back(term);

                consumeToken();

            } else {
                std::cerr << "Expected operator\n";
                exit(EXIT_FAILURE);
            }

        }
        return expr;
    }

private:
    std::vector<Token> m_inTokens;
    size_t m_index{};
    NodeProgram m_prog;

};

inline std::ostream& operator<< (std::ostream& out, NodeStatement* node) {
    if (std::holds_alternative<NodeStmtDefineVar*>(node->statement)) {
        return out << "NodeDefineVar";
    } else if (std::holds_alternative<NodeStmtAssignVar*>(node->statement)) {
        return out << "NodeAssignVar";
    } else if (std::holds_alternative<NodeStmtReturn*>(node->statement)) {
        return out << "NodeReturn";
    } else if (std::holds_alternative<NodeStmtScope*>(node->statement)) {
        return out << "NodeScope";
    } else {
        out << node->statement.index();
        return out << "add a case for Statement in overload of operator<<";
    }
}
#endif // PARSER_HPP
