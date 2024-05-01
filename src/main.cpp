#include <fstream>
#include <string>
#include <sstream>
#include <variant>
#include <vector>

#include "./Tokenizer.hpp"
#include "./Parser.hpp"

int main (int argc, char* argv[]){
    if (argc != 2) {
        std::cerr << "Incorrect usage.\n";
        std::cerr << "Usage: ./interpreter <file.gob>\n";
        return EXIT_FAILURE;
    }
    std::string file;
    
    {
        std::fstream inputFile(argv[1], std::ios::in);
        if (!inputFile) {
            std::cerr << "File could not be read.\n";
            return EXIT_FAILURE;
        }
        std::stringstream contents;
        contents << inputFile.rdbuf();
        file = contents.str();
    }
 
    Tokenizer tokenizer(file);
    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    const NodeProgram prog { parser.parse() };


    std::cout << "----------\n";
    for (Token token : tokens) {
        std::cout << token.type << '\n';
    }
    std::cout << "----------\n";

    /* TODO: what 
    for (size_t i {}; i < prog.statements.size(); ++i) {
        if (std::holds_alternative<NodeDefineVar*>(prog.statements[i]->statement)) {

            std::cout << prog.statements[i]->statement.index() << " NodeDefineVar\n";
            if (! std::get<0>(prog.statements[i]->statement)->value.has_value()) {
                std::cout << " no value in NodeDefineVar\n";
                continue;
            } 
            const auto& what { std::get<0>(prog.statements[i]->statement)->value.value()->expression };
            const auto& what2 { std::get<NodeBinaryExpr*>(what) };
            if (
                std::holds_alternative<NodeBinaryExpr*>(what)
                && std::holds_alternative<NodeBinaryExprAdd*>(what2->binaryExpr)
            ) {
                std::cout << " NodeBinaryExpressionAdd in NodeDefineVar\n";
            } else {
                std::cout << " no BinaryAdd present in NodeDefineVar\n";
            }



        } else if (std::holds_alternative<NodeAssignVar*>(prog.statements[i]->statement)) {
            const auto& what { prog.statements[i]->statement };
            const auto& what2 { std::get<1>(std::get<1>(what)->value->expression) };

            std::cout << what.index() << " NodeAssignVar\n";

            if (
                std::holds_alternative<NodeBinaryExpr*>(
                std::get<1>(what)->value->expression
                )
                &&
                std::holds_alternative<NodeBinaryExprAdd*>(what2->binaryExpr)
            ) {
                std::cout << " NodeBinaryExpressionAdd in NodeAssignVar\n";
            } else {
                std::cout << " no BinaryAdd present in NodeAssignVar\n";
            }


        } else if (std::holds_alternative<NodeReturn*>(prog.statements[i]->statement)) {
            const auto& what { prog.statements[i]->statement };
            const auto& what2 { std::get<1>(std::get<2>(what)->value->expression) };

            std::cout << prog.statements[i]->statement.index() << " NodeReturn\n";
            if (
                std::holds_alternative<NodeBinaryExpr*>(
                std::get<1>(what)->value->expression
                )
                &&
                std::holds_alternative<NodeBinaryExprAdd*>(what2->binaryExpr)
            ) {
                std::cout << " NodeBinaryExpressionAdd in NodeAssignVar\n";
            } else {
                std::cout << " no BinaryAdd present in NodeAssignVar\n";
            }



        } else if (std::holds_alternative<NodeScope*>(prog.statements[i]->statement)) {
            std::cout << prog.statements[i]->statement.index() << " NodeScope\n";


        } else {
            std::cout <<  prog.statements[i]->statement.index() <<
            "add a case for Statement in overload of operator<<\n";
        }

    }
    */
    
    std::cout << "----------\n";
    return EXIT_SUCCESS;
}
