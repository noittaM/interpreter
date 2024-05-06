#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "./Tokenizer.hpp"
#include "./Parser.hpp"
#include "Executer.hpp"

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


    // std::cout << "tokens: \n";
    // for (Token token : tokens) {
    //     std::cout << token.type << '\n';
    // }
    //
    // std::cout << "statemenst: \n";
    // for (auto node: prog.statements) {
    //     std::cout << node << '\n';
    // }

    Executer executer { prog };
    executer.execute();

    return EXIT_SUCCESS;
}
