#include <iostream>
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
        std::fstream inFile(argv[1], std::ios::in);
        if (!inFile) {
            std::cerr << "File could not be read.\n";
            return EXIT_FAILURE;
        }
        std::stringstream contents;
        contents << inFile.rdbuf();
        file = contents.str();
    }

    Tokenizer tokenizer(file);
    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    const std::string code { parser.parse() };
   
    return EXIT_SUCCESS;
}