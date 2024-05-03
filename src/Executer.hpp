#ifndef EXECUTER_HPP
#define EXECUTER_HPP

#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <iterator>
#include <optional>
#include <variant>
#include <vector>
#include "./Parser.hpp"

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
            std::cout << "assigning variable lwl" << std::endl;
        } else if (std::holds_alternative<NodeReturn*>(node->statement)) {
            std::cout << "returning variable lwl" << std::endl;
        } else if (std::holds_alternative<NodeScope*>(node->statement)) {
            std::cout << "how is there a scope lwl" << std::endl;
        } else {
            std::cerr << "don't know what this node is, dying...\n";
        }
    }

    void defineVariable(const NodeDefineVar* const node) {
        std::cout << "defining variable lwl" << std::endl;

        const std::string& ident = node->identifier;
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
            if (std::holds_alternative<NodeTerm *>(node->value.value()->expression)) {
                std::cout << "define variable has a binary expression" << std::endl;
                int value { evaluateTerm(std::get<NodeTerm *>(node->value.value()->expression)) };
                m_vars.push_back(Variable{ident, value });
            } else if (std::holds_alternative<NodeBinaryExpr *>(node->value.value()->expression)) {
                std::cout << "define variable has a binary expression" << std::endl;
                // int value { evaluateBinaryEpxression(
                //             std::get<NodeTerm *>(node->value.value()->expression)
                // )};
                // m_vars.push_back(Variable{ident, value });
            }
        } else {
            m_vars.push_back(Variable{ ident, {} });
        }

    }

    int evaluateTerm(const NodeTerm* term) const {
        if (std::holds_alternative<int_literal>(term->term)) {
            return std::get<int_literal>(term->term);

        } else if (std::holds_alternative<const identifier>(term->term)) {
            const std::string& ident { std::get<const identifier>(term->term) };
            const auto it { std::find_if(
                m_vars.begin(), m_vars.end(),
                [&ident] (const Variable& var)
                { return var.ident == ident; }
            )};

            if (it == m_vars.end()) {
                std::cerr << "error: use of undefined varialbe" << ident << '\n';
            }

            if (!it->value.has_value()) {
                std::cerr << "error: use of uninitialized variable" << ident << '\n';
            }

            return it->value.value();
        } else {
            std::cerr << "term has unknown variant\n";
            exit(EXIT_FAILURE);
        }
    }

    // int evaluateBinaryExpression()
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
