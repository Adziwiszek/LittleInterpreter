#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <variant>
#include <optional>
#include <map>

using std::string, std::cout, std::endl, std::cerr;

string substring(string str, int start, int end) {
    string res = "";
    for(int i = start; i < end; i++) {
        res += str[i];     
    }
    return res;
}
float strToFloat(string str) {
    int i = 0;
    int len = str.length();
    float res = 0;
    int pow = 1;
    for(; str[i] != '.' && str[i] != '\0'; i++) {} 
    for(int j = i - 1; j >= 0; j--) {
        res += (str[j] - '0') * pow;
        pow *= 10;
    }
    pow = 2;
    for(int j = i + 1; j < len; j++) {
        res += (str[j] - '0') * (1.0 / pow);
        pow = pow << 1;
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
    std::variant<float, bool, string> value;
    Literal(float v): value {v} {}
    Literal(bool v): value {v} {}
    Literal(const string& v): value {v} {}
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
        } else if(std::holds_alternative<float>(value)) {
            return std::to_string(get<float>());
        } else if(std::holds_alternative<bool>(value)) {
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
        return tokenTypeToString(type) + " " + lexme + " " + literalStr;
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
    std::map<string, TokenType> keywords;

    Scanner(string source_): tokens {}, source {source_}, keywords {} {
        setupKeywords(); 
    }
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
            case '"': stringLex(); break;
            default: 
                if(isDigit(c)) {
                    numberLex();
                } else if(isalpha(c)) {
                    identifierLex();                
                } else {
                    error(line, "Unexpected character!"); 
                }
                break;
        }
    }
    void identifierLex() {
        while (isAlphaNumeric(peek())) advance();
        string text = substring(source, start, current);
        TokenType type = keywords.contains(text) ? keywords[text] : IDENTIFIER;
        addToken(type); 
    }
    void numberLex() {
        while(isDigit(peek())) advance();
        if(peek() == '.' && isDigit(peekNext())) {
            // we consume '.'
            advance();
            while(isDigit(peek())) advance();
        } 
        std::optional<Literal> literalopt = 
            Literal(std::stof(substring(source, start, current)));
        addToken(NUMBER, literalopt); 
    }
    char peekNext() {
        if(current + 1 >= source.length()) return '\0';
        return source[current + 1];
    }
    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }
    bool isAlphaNumeric(char c) {
        return isDigit(c) || isalpha(c);
    }
    char peek() {
        if(isAtEnd()) return '\0';
        return source[current];
    }
    void stringLex() {
        while(peek() != '"' && !isAtEnd()) {
            if(peek() == '\n') line++;
            advance();
        }
        if(isAtEnd()) {
            error(line, "Undetermined string!");
        }
        advance();
        string finalStr = substring(source, start + 1, current - 1);
        std::optional<Literal> literalopt { std::optional<Literal>(finalStr) };
        addToken(STRING, literalopt);
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
    void setupKeywords();
};

// Expr abstract class---------------------------------------------------------

class Expr {
public:
    virtual ~Expr() = 0;
};

class Binop : public Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    Token op;
public:
    Binop(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left { std::move(left) }, right { std::move(right) }, op { op }
    { }
    ~Binop() override { }
};

class Unop : public Expr {
    std::unique_ptr<Expr> expr;
    Token op;
public:
    Unop(std::unique_ptr<Expr> expr, Token op)
        : expr { std::move(expr) }, op { op }
    { }
    ~Unop() override { }
};

class Grouping : public Expr {
    std::unique_ptr<Expr> expr;
public:
    Grouping(std::unique_ptr<Expr> expr) : expr { std::move(expr) } {}
    ~Grouping() override { }
};

class LiteralExpr : public Expr {
    std::unique_ptr<Literal> value;
public:
    LiteralExpr(std::unique_ptr<Literal> value) : value { std::move(value) } {}
    ~LiteralExpr() override { }
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

void Scanner::setupKeywords() {
    keywords["and"] = AND;
    keywords["class"] = CLASS;
    keywords["else"] = ELSE;
    keywords["false"] = FALSE;
    keywords["for"] = FOR;
    keywords["fun"] = FUN;
    keywords["if"] = IF;
    keywords["nil"] = NIL;
    keywords["or"] = OR;
    keywords["print"] = PRINT;
    keywords["return"] = RETURN;
    keywords["super"] = SUPER;
    keywords["this"] = THIS;
    keywords["true"] = TRUE;
    keywords["var"] = VAR;
    keywords["while"] = WHILE; 
}


