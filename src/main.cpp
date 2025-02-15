#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <variant>
#include <optional>

using std::string, std::cout, std::endl, std::cerr;

string substring(string str, int start, int end) {
    string res = "";
    for(int i = start; i < end; i++) {
        res += str[i];     
    }
    return res;
}
// Interpreter stuff ----------------------------------------------------------
bool hadError { false };
void runFile(std::string path);
void runPrompt();
void run(std::string source);
void error(int line, string message);
void report(int line, string where, string message);
// Literal --------------------------------------------------------------------
struct Literal {
    std::variant<int, bool, string> value;
    Literal(int v): value {v} {}
    Literal(bool v): value {v} {}
    Literal(string v): value {v} {}
    template <typename T>
    T get() const {
        return std::get<T>(value);
    }
    void print() {
        std::visit([](auto&& arg) { std::cout << arg << '\n'; }, value);
    }
    string toString() const {
        if(std::holds_alternative<string>(value)) {
            return get<string>();
        } else if(std::holds_alternative<string>(value)) {
            return std::to_string(get<int>());
        } else if(std::holds_alternative<string>(value)) {
            return get<bool>() ? "1" : "0";
        } 
        return "";
    }
};
// Token ----------------------------------------------------------------------
enum TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    EOF_
};
std::string tokenTypeToString(TokenType type);
class Token {
public:
    TokenType type;
    std::string lexme;
    std::optional<Literal> literal;
    int line;

    Token(TokenType type_, std::string lexme_, int line_, std::optional<Literal> literal_) :
        type {type_}, lexme {lexme_}, line {line_}, literal {literal_}
    {}
    Token(TokenType type_, std::string lexme_, int line_) :
        Token(type_, lexme_, line_, std::nullopt) 
    {}

    std::string toString() const {
        string literalStr = literal.has_value() ? literal->toString() : "";
        return tokenTypeToString(type) + " " + lexme + literalStr;
    }
};
// Scanner ---------------------------------------------------------------------
class Scanner {
public:
    string source;
    std::vector<Token> tokens;
    int start { 0 };
    int current { 0 };
    int line { 1 };

    Scanner(string source_): tokens {}, source {source_} {}
    std::vector<Token> scanTokens() {
        while(!isAtEnd()) {
            start = current;
            scanToken();
        }
        tokens.push_back(Token(EOF_, "", line));
        return tokens;
    }
    void scanToken() {
        char c = advance();
        switch (c) {
            case '(': addToken(LEFT_PAREN); break;
            case ')': addToken(RIGHT_PAREN); break;
            case '{': addToken(LEFT_BRACE); break;
            case '}': addToken(RIGHT_BRACE); break;
            case ',': addToken(COMMA); break;
            case '.': addToken(DOT); break;
            case '-': addToken(MINUS); break;
            case '+': addToken(PLUS); break;
            case '*': addToken(STAR); break; 
            case ';': addToken(SEMICOLON); break;
            case '!':
                addToken(match('=') ? BANG_EQUAL : BANG);
                break;
            case '=':
                addToken(match('=') ? EQUAL_EQUAL : EQUAL);
                break;
            case '<':
                addToken(match('=') ? LESS_EQUAL : LESS);
                break;
            case '>':
                addToken(match('=') ? GREATER_EQUAL : GREATER);
                break;
            case '/':
                if (match('/')) {
                  // A comment goes until the end of the line.
                  while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                  addToken(SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            case '\n':
                line++;
                break;
            default: error(line, "Unexpected character!"); break;
        }
    }
    char peek() {
        if(isAtEnd()) return '\0';
        return source[current];
    }

    bool match(char expected) {
        if(isAtEnd()) return false;
        if(source[current] != expected) return false;
        current++;
        return true;
    }
    char advance() {
        return source[current++];
    }
    void addToken(TokenType type) {
        addToken(type, std::nullopt);
    }
    void addToken(TokenType type, std::optional<Literal> literal) {
        string text = substring(source, start, current);
        tokens.push_back(Token(type, text, line, literal));
    }
    bool isAtEnd() {
        return current >= source.length();
    }

};

int main(int argc, char** argv) {
    if(argc > 2) {
        std::cout << "Usage: ./dupa [script]" << std::endl;
    } else if(argc == 2) {
        runFile(std::string(argv[1]));
    } else {
        runPrompt();
    }
    return 0;
}


std::string tokenTypeToString(TokenType type) {
    switch(type) {
        case LEFT_PAREN: return "LEFT_PAREN"; break;
        case RIGHT_PAREN: return "RIGHT_PAREN"; break;
        case LEFT_BRACE: return "LEFT_BRACE"; break;
        case RIGHT_BRACE: return "RIGHT_BRACE"; break;
        case COMMA: return "COMMA"; break;
        case DOT: return "DOT"; break;
        case MINUS: return "MINUS"; break;
        case PLUS: return "PLUS"; break;
        case SEMICOLON: return "SEMICOLON"; break;
        case SLASH: return "SLASH"; break;
        case STAR: return "STAR"; break;
        case BANG: return "BANG"; break;
        case BANG_EQUAL: return "BANG_EQUAL"; break;
        case EQUAL: return "EQUAL"; break;
        case EQUAL_EQUAL: return "EQUAL_EQUAL"; break;
        case GREATER: return "GREATER"; break;
        case GREATER_EQUAL: return "GREATER_EQUAL"; break;
        case LESS: return "LESS"; break;
        case LESS_EQUAL: return "LESS_EQUAL"; break;
        case IDENTIFIER: return "IDENTIFIER"; break;
        case STRING: return "STRING"; break;
        case NUMBER: return "NUMBER"; break;
        case AND: return "AND"; break;
        case CLASS: return "CLASS"; break;
        case ELSE: return "ELSE"; break;
        case FALSE: return "FALSE"; break;
        case FUN: return "FUN"; break;
        case FOR: return "FOR"; break;
        case IF: return "IF"; break;
        case NIL: return "NIL"; break;
        case OR: return "OR"; break;
        case PRINT: return "PRINT"; break;
        case RETURN: return "RETURN"; break;
        case SUPER: return "SUPER"; break;
        case THIS: return "THIS"; break;
        case TRUE: return "TRUE"; break;
        case VAR: return "VAR"; break;
        case WHILE: return "WHILE"; break;
        case EOF_: return "EOF_"; break;
    }
    return "unknow token type";
}

void runFile(std::string path) {
    std::ifstream input(path);
    if(!input.good()) {
        std::cerr << "runFile(path) error\nFailed to open " << path << std::endl;
        return;
    }
    std::stringstream buffer;
    buffer << input.rdbuf();
    run(buffer.str());
}

void runPrompt() {
    std::string userInput;
    while(true) {
        std::cout << "> ";
        if(!std::getline(std::cin, userInput)) break;
        run(userInput);
        hadError = false;
    }
}

void run(std::string source) {
    if(hadError) return;
    Scanner scanner(source);
    auto tokens = scanner.scanTokens();
    for(const auto& token: tokens) {
        std::cout << token.toString() << std::endl;
    }
}

void error(int line, string message) {
    report(line, "", message);
}

void report(int line, string where, string message) {
    cerr << "[line " << line << "] Error " << where << ": " << message << endl;
    hadError = true;
}
