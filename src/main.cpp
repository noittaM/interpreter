#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "./Tokenizer.hpp"
#include "./Parser.hpp"






// std::string tokenToStr (const std::vector<Token> tokens) {
//     std::string output{};
//     for (int i{0}; i < tokens.size(); ++i) {
//         const Token& token = tokens[i]; 
//         switch (token.type)
//         {
//         case TokenType::_return:
//             if (i + 1 < tokens.size() && (tokens[i+1].type == TokenType::int_lit || tokens[i+1].type == TokenType::expression))
//                 if (i + 2 < tokens.size () && tokens[i+2].type == TokenType::semi) {
//                     output += "return " + tokens[i+1].value + ";\n"; 
//                     i += 2;
//                 }
//                 else {
//                     std::cerr << "Expected ';'.\n";
//                     exit(EXIT_FAILURE);
//                 }
//             else {
//             std::cerr << "Expected expression.\n";
//             exit(EXIT_FAILURE);
//             }
//             break;

//         case TokenType::int_lit:
//             std::cerr << "What do I do with int literal: " << token.value << "????.\n";
//             exit(EXIT_FAILURE);

//         case TokenType::semi:
//             std::cerr << "What do I do with semi: ';'????.\n";
//             exit(EXIT_FAILURE);
        
//         default:
//             std::cerr << "No idea what this token is.\n";
//             std::cerr << "Token value: '" << token.value << "'.\n";
//             std::cerr << "Token type: '" << token.type << "'.\n";
//             exit(EXIT_FAILURE);
//             break;
//         }
//     }
//     return output;
// }

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