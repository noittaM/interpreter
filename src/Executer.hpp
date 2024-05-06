#ifndef EXECUTER_HPP
#define EXECUTER_HPP

#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <variant>
#include <vector>
#include "./Parser.hpp"
#include "Tokenizer.hpp"

struct Variable{
    std::string ident;
    std::optional<int> value;
};

class Executer {

public:
    Executer(NodeProgram prog)
    : m_prog { prog }
    {
    }

    void execute() {
        while (peekNode().has_value()) {
            executeNode(peekNode().value()); 
            consumeNode();
        }
    }

private:
    void executeNode(const NodeStatement* node) {
        if (std::holds_alternative<NodeDefineVar*>(node->statement)) {
            defineVariable(std::get<NodeDefineVar *>(node->statement));

        } else if (std::holds_alternative<NodeAssignVar*>(node->statement)) {
            assignVariable(std::get<NodeAssignVar*>(node->statement));

        } else if (std::holds_alternative<NodeReturn*>(node->statement)) {
            returnExpression(std::get<NodeReturn *>(node->statement));
        } else if (std::holds_alternative<NodeScope*>(node->statement)) {
            std::cerr << "scope is not implemented yet\n";
            exit(EXIT_FAILURE);
        } else {
            std::cerr << "statment has unexpected variant";
        }
    }

    void defineVariable(const NodeDefineVar* const node) {
        const std::string ident = node->identifier;
        const auto it { // what the hell type is this
            std::find_if(
                    m_vars.begin(),
                    m_vars.end(),
                    [&ident](const Variable& var)
                { return ident == var.ident; })
        };
        if (it != m_vars.end()) {
            std::cerr << "Variable " << ident << " already exists.\n";
            exit(EXIT_FAILURE);
        }

        if (node->value.has_value()) {
            int value { evaluateExpression(node->value.value()) };
            m_vars.push_back(Variable { ident, value });
        } else {
            m_vars.push_back(Variable{ ident, {} });

        }
    }

    void assignVariable(const NodeAssignVar* const node) {

        const std::string ident = node->identifier;
        const int value { evaluateExpression(node->value) };

        const auto it { // what the hell type is this
            std::find_if(
                    m_vars.begin(),
                    m_vars.end(),
                    [&ident](const Variable& var)
                { return ident == var.ident; })
        };

        if (it == m_vars.end()) {
            std::cerr << "variable " << ident << " not intialized.";
            exit(EXIT_FAILURE);
        }

        it->value = value;
    }

    void returnExpression(const NodeReturn* const node) const {
        int value { evaluateExpression(node->value) };
        exit(value); // FIXME: horrible
    }

    int evaluateTerm(const NodeTerm* const term) const {
        if (std::holds_alternative<int_literal>(term->term)) {
            return std::get<int_literal>(term->term);

        } else if (std::holds_alternative<const identifier>(term->term)) {
            const std::string& ident { std::get<const identifier>(term->term) };
            const auto it { std::find_if( // check if the identifer exists
                m_vars.begin(), m_vars.end(),
                [&ident] (const Variable& var)
                { return var.ident == ident; }
            )};

            if (it == m_vars.end()) {
                std::cerr << "error: use of undefined variable" << ident << '\n';
                exit(EXIT_FAILURE);
            }

            if (!it->value.has_value()) {
                std::cerr << "error: use of uninitialized variable" << ident << '\n';
                exit(EXIT_FAILURE);
            }


            return it->value.value();
        } else {
            // unreachable?
            std::cerr << "term has unknown variant\n";
            exit(EXIT_FAILURE);
        }
    }

    int evaluateBinaryEpxression(const NodeBinaryExpr* const expr) const {
        // evaluates left to right for now.

        if (expr->operands.size() != expr->operators.size() + 1) {
            std::cerr << "wrong number of operatos for number of operands\n"
            << "number of operators: " << expr->operators.size()
            << "\nnumberof operands: " << expr->operands.size();
        }

        int valueOfExpression { evaluateTerm(expr->operands.at(0)) };

        for (std::size_t i {}; i < expr->operators.size(); ++i) {
            switch (expr->operators.at(i)) {
                case TokenType::plus:
                    valueOfExpression += evaluateTerm(expr->operands.at(i+1));
                    continue;

                case TokenType::dash:
                    valueOfExpression -= evaluateTerm(expr->operands.at(i+1));
                    continue;

                case TokenType::forward_slash:
                    int devisor;
                    devisor = evaluateTerm(expr->operands.at(i+1));
                    if (devisor == 0) {
                        std::cerr << "attempting to devide by 0...dying...\n";
                        exit(EXIT_FAILURE);
                    }

                    valueOfExpression /= devisor;
                    continue;

                case TokenType::asterisk:
                    valueOfExpression *= evaluateTerm(expr->operands.at(i+1));
                    continue;

                default:
                    std::cerr <<
                        "trying to evaluate binary expression "
                        << "with operators of wrong type\n";
                    exit(EXIT_FAILURE);
            }
        }

        return valueOfExpression;
    }

    int evaluateExpression(const NodeExpr* const expr) const {
        if (std::holds_alternative<NodeTerm *>(expr->expression)) {
            return evaluateTerm(std::get<NodeTerm *>(expr->expression));
        }

        if (std::holds_alternative<NodeBinaryExpr *>(expr->expression)) {
            return evaluateBinaryEpxression(
                std::get<NodeBinaryExpr *>(expr->expression));
        }

        std::cerr << "expression holds unexpected variant\n";
        exit(EXIT_FAILURE);
    }

    void consumeNode() {
        ++m_index;
    }

    std::optional<NodeStatement*> peekNode() const {
        if (m_index < m_prog.statements.size()) {
            return m_prog.statements[m_index];
        }
        return {};
    }

private:
NodeProgram m_prog;
size_t m_index{};
std::vector<Variable> m_vars;
};

#endif // EXECUTER_HPP
