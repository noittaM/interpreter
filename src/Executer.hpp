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
    ident identifier;
    std::optional<int> value;
};

class Executer {

public:
    Executer(NodeProgram prog)
    : m_prog { prog }
    {
    }

    void execute() {
        while (peekStmt().has_value()) {
            executeStmt(peekStmt().value()); 
            consumeStmt();
        }
    }

private:
    void executeStmt(const NodeStatement* node) {
        if (std::holds_alternative<NodeStmtDefineVar *>(node->statement)) {
            executeStmtDefineVar(std::get<NodeStmtDefineVar *>(node->statement));

        } else if (std::holds_alternative<NodeStmtAssignVar *>(node->statement)) {
            executeStmtAssignVariable(std::get<NodeStmtAssignVar *>(node->statement));

        } else if (std::holds_alternative<NodeStmtReturn *>(node->statement)) {
            executreStmtReturnExpression(std::get<NodeStmtReturn *>(node->statement));

        } else if (std::holds_alternative<NodeStmtScope *>(node->statement)) {
            executeStmtScope(std::get<NodeStmtScope *>(node->statement));

        } else {
            std::cerr << "statment has unexpected variant";
        }
    }

    void executeStmtDefineVar(const NodeStmtDefineVar* const node) {
        const std::string ident = node->identifier;
        std::vector<Variable>::iterator it { // what the hell type is this
            std::find_if(
                    m_vars.begin(),
                    m_vars.end(),
                    [&ident](const Variable& var)
                { return ident == var.identifier; })
        };
        if (it != m_vars.end()) {
            std::cerr << "Variable " << ident << " already exists."
                << std::endl;
            exit(EXIT_FAILURE);
        }

        if (node->value.has_value()) {
            int value { evaluateExpression(node->value.value()) };
            m_vars.push_back(Variable { ident, value });
        } else {
            m_vars.push_back(Variable{ ident, {} });

        }
    }

    void executeStmtAssignVariable(const NodeStmtAssignVar* const node) {

        const std::string ident = node->identifier;
        const int value { evaluateExpression(node->value) };

        std::vector<Variable>::iterator it { // what the hell type is this
            std::find_if(
                    m_vars.begin(),
                    m_vars.end(),
                    [&ident](const Variable& var)
                { return ident == var.identifier; })
        };

        if (it == m_vars.end()) {
            std::cerr << "variable " << ident << " not intialized.";
            exit(EXIT_FAILURE);
        }

        it->value = value;
    }

    void executreStmtReturnExpression(const NodeStmtReturn* const node) const {
        int value { evaluateExpression(node->value) };
        exit(value); // FIXME: horrible
    }

    void executeStmtScope(const NodeStmtScope* const node) {
        m_scopes.push_back(m_vars.size());

        for (const NodeStatement* stmt : node->statements) {
            executeStmt(stmt); 
        }

        // pop variables defined in scope
        for (std::size_t i = m_vars.size() - 1; i >= m_scopes.back(); --i) {
            m_vars.pop_back();
        }
        m_scopes.pop_back();
    }

    int evaluateTerm(const NodeTerm* const term) const {
        if (std::holds_alternative<int_literal>(term->term)) {
            return std::get<int_literal>(term->term);

        } else if (std::holds_alternative<ident>(term->term)) {
            ident& identifier { std::get<ident>(term->term) };
            std::vector<Variable>::const_iterator it { std::find_if( // check if the identifer exists
                m_vars.begin(), m_vars.end(),
                [&identifier] (const Variable& var)
                { return var.identifier == identifier; }
            )};

            if (it == m_vars.end()) {
                std::cerr << "error: use of undefined variable " << identifier
                    << std::endl;
                exit(EXIT_FAILURE);
            }

            if (!it->value.has_value()) {
                std::cerr << "error: use of uninitialized variable " << identifier
                    << std::endl;
                exit(EXIT_FAILURE);
            }


            return it->value.value();
        } else {
            // unreachable?
            std::cerr << "term has unknown variant" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    int evaluateBinaryEpxression(const NodeBinaryExpr* const expr) const {
        // evaluates left to right for now.

        if (expr->operands.size() != expr->operators.size() + 1) {
            std::cerr << "wrong number of operatos for number of operands\n"
                << "number of operators: " << expr->operators.size()
                << "numberof operands: " << expr->operands.size() << std::endl;
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
                        std::cerr << "attempting to devide by 0...dying..." << std::endl;
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
                        << "with operators of wrong type" << std::endl;
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

        std::cerr << "expression holds unexpected variant" << std::endl;
        exit(EXIT_FAILURE);
    }

    void consumeStmt() {
        ++m_index;
    }

    std::optional<NodeStatement *> peekStmt() const {
        if (m_index < m_prog.statements.size()) {
            return m_prog.statements[m_index];
        }
        return {};
    }

private:
    NodeProgram m_prog;
    std::size_t m_index{};
    std::vector<std::size_t> m_scopes;
    std::vector<Variable> m_vars;
};

#endif // EXECUTER_HPP
