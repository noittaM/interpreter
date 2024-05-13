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

struct NodeBinaryExpr {
    std::vector<TokenType> operators;
    std::vector<NodeTerm *> operands;
};

struct NodeStatement;

struct NodeScope {
    std::vector <NodeStatement *> statements;
};

struct NodeExpr {
    std::variant <NodeTerm *, NodeBinaryExpr *> expression;
};

struct NodeDefineVar{
    const std::string identifier;
    std::optional <NodeExpr *> value; // TODO: Types
};

struct NodeAssignVar {
    const std::string identifier;
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
        m_index++;
    }

    std::optional<NodeStatement *> getNodeStatement() {
        NodeStatement* stmt;

        if (std::optional<NodeDefineVar *> node = getNodeDefineVar()) {
            stmt = new NodeStatement;
            stmt->statement = node.value();
            return stmt;
        } else if (std::optional<NodeAssignVar *> node = getNodeAssignVar()) {
            stmt = new NodeStatement;
            stmt->statement = node.value();
            return stmt;
        } else if (std::optional<NodeReturn *> node = getNodeReturn()) {
            stmt = new NodeStatement;
            stmt->statement = node.value();
            return stmt;
        } else {
            return {};
        }
    }

    std::optional<NodeDefineVar *> getNodeDefineVar() {
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
            NodeDefineVar* node { new NodeDefineVar {
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
        NodeDefineVar* node { new NodeDefineVar { ident, expr.value() } };
        return node;
    }

    std::optional<NodeAssignVar *> getNodeAssignVar() {
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
        NodeAssignVar* node { new NodeAssignVar { ident, expr.value() } };
        return node;
    }

    std::optional<NodeReturn *> getNodeReturn() {
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

        NodeReturn* node { new NodeReturn { expr.value() } };
        return node;
    }

    std::optional<NodeScope *> getNodeScope() {
        if (!peekToken().has_value()
            || peekToken().value().type != TokenType::curly_open
        ) {
            return {};
        }
        consumeToken();

        NodeScope* scope { new NodeScope };
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
            term->term.emplace<const identifier>(peekToken().value().value.value());
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
            if (!peekToken().value().value.has_value()) {
                std::cerr << "parser error: got identifier with no value\n";
            }

            expr = new NodeBinaryExpr;
            NodeTerm* term { new NodeTerm { peekToken().value().value.value() }};

            expr->operands.push_back(term);

            consumeToken();
        } else if (peekToken().value().type == TokenType::int_lit) {
            if (!peekToken().value().value.has_value()) {
                std::cerr << "parser error: got int literal with no value\n";
            }

            expr = new NodeBinaryExpr;
            NodeTerm* term { new NodeTerm { std::stoi(peekToken().value().value.value()) }};

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

                NodeTerm* term { new NodeTerm { peekToken().value().value.value() }};
                expr->operands.push_back(term);

                consumeToken();

            } else if (peekToken().value().type == TokenType::int_lit) {
                if (!peekToken().value().value.has_value()) {
                    std::cerr << "parser error: got int literal with no value\n";
                }

                NodeTerm* term { new NodeTerm { std::stoi(peekToken().value().value.value()) }};
                expr->operands.push_back(term);

                consumeToken();

            } else {
                std::cerr << "Expected operator\n";
                exit(EXIT_FAILURE);
            }

        }
        return expr;
    }
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
