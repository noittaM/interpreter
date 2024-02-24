#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

enum class TokenType {
    _return,
    int_lit,
    expression,
    semi,
    plus,
};

struct Token
{
    const TokenType type;
    const std::string value;
};

std::ostream& operator<< (std::ostream& out, const TokenType value) {
    switch (value) {
        case TokenType::_return:
            return out << "_return";
        case TokenType::int_lit:
            return out << "int_lit";
        case TokenType::expression:
            return out << "expression";
        case TokenType::semi:
            return out << "semi";
        case TokenType::plus:
            return out << "plus";
        default:
            return out << "add case in operator<<\n";
    }
}
std::vector<Token> tokenize (const std::string& str) {
    std::vector<Token> tokens{};
    std::string buf{};
    char c{};
    
    for (size_t i{0}; i < str.size(); ++i) {
        c = str[i];
        if (std::isalpha(c)) {
            while (std::isalnum(str[i])){
                buf.push_back(str[i]);
                ++i;
            }           
            --i;
            if (buf == "return") {
                tokens.push_back({ .type = TokenType::_return, .value = "return" });
                buf.clear();
                continue;
            } else {
                std::cerr << "Unrecognized string: '" << buf << "',dying...\n";
                exit(EXIT_FAILURE); 
            }

        } else if (std::isdigit(c)) {
            while(std::isdigit(str[i])) {
                buf.push_back(str[i]);
                ++i;
            }
            if (i < str.size() && str[i] == '+') {
                while (i < str.size() && str[i] == '+') {
                    buf.push_back('+');
                    ++i;
                    while (std::isdigit(str[i])){
                        buf.push_back(str[i]);
                        ++i;
                    }
                }
                std::cout<<"broke\n";

                tokens.push_back({ .type = TokenType::expression, .value = buf });
                buf.clear();
            } else {
                tokens.push_back({ .type = TokenType::int_lit, .value = buf });
                buf.clear();   
            }
            --i;
            continue;
        } else if (std::isspace(c)) {
            continue;
        }

        if (c == ';') {
            tokens.push_back({ .type = TokenType::semi, .value = ";"});
            buf.clear();
        } else if (c == '+') {
            tokens.push_back({ .type = TokenType::plus, .value = "+"});
        } else {
            std::cerr << "Unrecognized symbol: '" << c << "', dying rn...\n";
            exit(EXIT_FAILURE);
        }
        
    }

    // then look for expressions (?seems overcomplicated)
    for (int i{0}; i < tokens.size(); ++i) {

    }
    return tokens;
} 


std::string tokenToStr (const std::vector<Token> tokens) {
    std::string output{};
    for (int i{0}; i < tokens.size(); ++i) {
        const Token& token = tokens[i]; 
        switch (token.type)
        {
        case TokenType::_return:
            if (i + 1 < tokens.size() && (tokens[i+1].type == TokenType::int_lit || tokens[i+1].type == TokenType::expression))
                if (i + 2 < tokens.size () && tokens[i+2].type == TokenType::semi) {
                    output += "return " + tokens[i+1].value + ";\n"; 
                    i += 2;
                }
                else {
                    std::cerr << "Expected ';'.\n";
                    exit(EXIT_FAILURE);
                }
            else {
            std::cerr << "Expected expression.\n";
            exit(EXIT_FAILURE);
            }
            break;

        case TokenType::int_lit:
            std::cerr << "What do I do with int literal: " << token.value << "????.\n";
            exit(EXIT_FAILURE);

        case TokenType::semi:
            std::cerr << "What do I do with semi: ';'????.\n";
            exit(EXIT_FAILURE);
        
        default:
            std::cerr << "No idea what this token is.\n";
            std::cerr << "Token value: '" << token.value << "\'.\n";
            std::cerr << "Token type: '" << token.type << "\'.\n";
            exit(EXIT_FAILURE);
            break;
        }
        if (token.type == TokenType::_return) {
            
            } else if (token.type == TokenType::int_lit) {
                std::cerr << "What do I do with " << token.value << "????.\n";
                exit(EXIT_FAILURE);

            } else if (token.type == TokenType::semi) {
                std::cerr << "What do I do with a ';'????.\n";
                exit(EXIT_FAILURE);
            } else {
                std::cerr << "No idea what this token is\n";
                std::cerr << "Token value: '" << token.value << "\'.\n";
                std::cerr << "Token type: '" << token.type << "\'.\n";
                exit(EXIT_FAILURE);
            }
    
    }
    return output;
}

void writeAsCpp (const std::string& str) {
    std::string output;
    output = "int main(void) {\n";
    output += str;
    output += '}';
    system("touch out.cpp"); //TODO: should add a check or something so it overwrite.
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

    std::vector<Token> tokens = tokenize(file);
    for (auto var : tokens) {
        std::cout << var.type << ": "<< var.value << '\n';
    }
    const std::string code { tokenToStr(tokens) };
    writeAsCpp(code);
    

    return EXIT_SUCCESS;
}