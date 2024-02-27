#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "./Tokenizer.hpp"
#include "./Parser.hpp"

void writeAsCpp (const std::string& str) {
    std::string output;
    output = "int main() {\n";
    output += str;
    output += '}';
    system("touch out.cpp");
    std::ofstream file;
    file.open("out.cpp");
    file << output;
}


int main (int argc, char* argv[]){
    if (argc != 2) {
        std::cerr << "Incorrect usage.\n";
        std::cerr << "Usage: ./interpreter <file.gob>\n";
        return EXIT_FAILURE;
    }
    std::string file;
    
    {
        std::fstream inFile(argv[1], std::ios::in);
        if(!inFile) {
            std::cerr << "File could not be read.\n";
            return EXIT_FAILURE;
        }
        std::stringstream contents;
        contents << inFile.rdbuf();
        file = contents.str();
    }

    Tokenizer tokenizer(file);
    std::vector<Token> tokens = tokenizer.tokenize();
    for (auto var : tokens) {
        std::cout << var.type << ": "<< var.value.value() << '\n';
    }
    Parser parser(tokens);
    const std::string code { parser.parse() };
    writeAsCpp(code);
    
   
    return EXIT_SUCCESS;
}