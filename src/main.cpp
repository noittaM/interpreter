#include <fstream>
#include <string>
#include <sstream>
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
    for (Token token : tokens) {
        std::cout << token.type << '\n';
    }
    std::cout << "----------\n";
    Parser parser(tokens);
    const nodeProgram prog { parser.parse() };
    std::cout << "----------\n";
    for (size_t i {}; i < prog.statements.size(); ++i) {
        if (std::holds_alternative<NodeDefineVar*>(prog.statements[i]->statement)) {
            std::cout << prog.statements[i]->statement.index() << " NodeDefineVar\n";
        } else if (std::holds_alternative<NodeAssignVar*>(prog.statements[i]->statement)) {
            std::cout << prog.statements[i]->statement.index() << " NodeAssignVar\n";
        } else if (std::holds_alternative<NodeReturn*>(prog.statements[i]->statement)) {
            std::cout << prog.statements[i]->statement.index() << " NodeReturn\n";
        } else if (std::holds_alternative<NodeScope*>((prog.statements[i]->statement))) {
            std::cout << "NodeScope\n";
        } else {
            std::cout <<  prog.statements[i]->statement.index() <<
            "add a case for Statement in overload of operator<<\n";
        }

    }
    
    std::cout << "----------\n";
    return EXIT_SUCCESS;
}