#ifndef EXECUTER_HPP
#define EXECUTER_HPP

#include <algorithm>
#include <variant>
#include <vector>
#include "./Parser.hpp"

struct Variable{
    std::string ident;
    std::string value;
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
        }
    }

private:
    void executeNode(NodeStatement* node) {
        const auto& stmt = node->statement;

        if (std::holds_alternative<NodeDefineVar*>(stmt)) {
                const std::string ident = std::get<NodeDefineVar*>(stmt)->identifier;
                std::vector<std::string>::iterator it {
                    std::find_if(
                        m_vars.begin(),
                        m_vars.end(),
                        [&](const std::string& newVar){
                            return ident == newVar;
                        })};
                if (it == m_vars.end()) {
                    m_vars.push_back(ident);
                } else {
                    std::cerr << "Variable " << ident << " already exists.\n";
                    exit(EXIT_FAILURE);
                }
            } else if (std::holds_alternative<NodeAssignVar*>(stmt)) {

            } else if (std::holds_alternative<NodeReturn*>(stmt)) {

            } else if (std::holds_alternative<NodeScope*>(stmt)) {

            }
    }
    void take() {
        ++m_index;
    }

    std::optional<NodeStatement*> peekNode() {
        if (m_index < m_prog.statements.size()) {
            return m_prog.statements[m_index];
        }
        return {};
    }

NodeProgram m_prog;
size_t m_index{};
std::vector<Variable> m_vars;
};

#endif // EXECUTER_HPP