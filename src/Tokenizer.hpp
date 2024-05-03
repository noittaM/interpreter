#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <optional>


enum class TokenType {
    _return ,
    identifier,
    let,
    int_lit,
    curly_open,
    curly_close,
    semi,
    plus,
    dash,
    forward_slash,
    asterisk,
    equal
};


struct Token{
    const TokenType type;
    const std::optional<std::string> value;
};


class Tokenizer {
private:
    const std::string m_inString;
    size_t m_index{0};



    std::optional<char> peekChar() const {
        if(m_index < m_inString.length())
            return m_inString[m_index];
        return {};
    }

    void consumeChar() {
        m_index++;
    }

public:

    explicit Tokenizer (const std::string& code)
    : m_inString{code}
    {
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens{};
        std::string buf{};

        while(peekChar().has_value()) {
            if (std::isalpha(peekChar().value())) {
                while (peekChar().has_value() && std::isalnum(peekChar().value())) {
                    buf.push_back(peekChar().value());
                    consumeChar();
                }

                if (buf == "return") {
                    tokens.push_back({ .type = TokenType::_return, .value = {} });
                    buf.clear();
                    continue;

                } else if (buf == "let") {
                    tokens.push_back({ .type = TokenType::let, .value = {} });
                    buf.clear();
                    continue;

                } else {
                    tokens.push_back({ .type = TokenType::identifier, .value = buf});
                    buf.clear();
                    continue;

                }
            } else if (std::isdigit(peekChar().value())) {
                while(std::isdigit(peekChar().value())) {
                    buf.push_back(peekChar().value());
                    consumeChar();
                }

                tokens.push_back({ .type = TokenType::int_lit, .value = buf });
                buf.clear();
                continue;

            } else if (peekChar().value() == '+') {
                tokens.push_back({ .type = TokenType::plus, .value = {} });
                consumeChar();
                buf.clear();// TODO: why?
                continue;

            } else if (peekChar().value() == '-'){
                tokens.push_back({ .type = TokenType::dash, .value = {} });
                consumeChar();
                buf.clear();// TODO: why?
                continue;

            } else if (peekChar().value() == '*'){
                tokens.push_back({ .type = TokenType::asterisk, .value = {} });
                consumeChar();
                buf.clear();// TODO: why?
                continue;

            } else if (peekChar().value() == '/'){
                tokens.push_back({ .type = TokenType::forward_slash, .value = {} });
                consumeChar();
                buf.clear();// TODO: why?
                continue;

            } else if (peekChar().value() == '=') {
                tokens.push_back({ .type = TokenType::equal, .value = {} });
                consumeChar();
                buf.clear();
                continue;

            } else if (peekChar().value() == ';') {
                tokens.push_back({ .type = TokenType::semi, .value = {} });
                consumeChar();
                buf.clear();
                continue;
            } else if (peekChar().value() == '{') {
                tokens.push_back({ .type = TokenType::curly_open, .value = {} });
                consumeChar();
                buf.clear();
            } else if (peekChar().value() == '}') {
                tokens.push_back({ .type = TokenType::curly_close, .value = {} });
                consumeChar();
                buf.clear();
            } else if(std::isspace(peekChar().value())) {
                consumeChar();
                continue;

            } else {
                std::cerr << "Unrecognized symbol: '" << peekChar().value() << "', dying rn...\n";
                exit(EXIT_FAILURE);
            }
        }
        return tokens;
    }
};


inline std::ostream& operator<< (std::ostream& out, const TokenType value) {
    switch (value) {
        case TokenType::_return:
            return out << "_return";
        case TokenType::identifier:
            return out << "identifier";
        case TokenType::let:
            return out << "let";
        case TokenType::int_lit:
            return out << "int_lit";
        case TokenType::curly_open:
            return out << "curly_open";
        case TokenType::curly_close:
            return out << "curly_close";
        case TokenType::semi:
            return out << "semi";
        case TokenType::plus:
            return out << "plus";
        case TokenType::dash:
            return out << "dash";
        case TokenType::forward_slash:
            return out << "forward_slash";
        case TokenType::asterisk:
            return out << "asterisk";
        case TokenType::equal:
            return out << "equal";
        default:
            return out << "add a case for TokenType in overload of operator<<";
    }
}

#endif //TOKENIZER_HPP
