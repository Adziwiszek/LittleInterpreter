#include <iostream>
#include <any>
#include <functional>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <variant>
#include <optional>
#include <map>

using std::string, std::cout, std::endl, std::cerr;

struct Nil { Nil() {} };

// Literal --------------------------------------------------------------------
using Value = std::variant<float, bool, string, Nil>;
string valueToString(Value value) {
    if(std::holds_alternative<string>(value)) {
        return std::get<string>(value);
    } else if(std::holds_alternative<float>(value)) {
        return std::to_string(std::get<float>(value));
    } else if(std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "1" : "0";
    } else if(std::holds_alternative<Nil>(value)) {
        return "nil";
    }
    return "";
}
struct Literal {
    Value value;
    Literal(float v): value {v} {}
    Literal(bool v): value {v} {}
    Literal(const string& v): value {v} {}
    Literal(Nil v): value {v} {}
    template <typename T>
    T get() const {
        return std::get<T>(value);
    }
    /*void print() {
        std::visit([](auto&& arg) { std::cout << arg << '\n'; }, value);
    }*/
    string toString() const {
        if(std::holds_alternative<string>(value)) {
            return get<string>();
        } else if(std::holds_alternative<float>(value)) {
            return std::to_string(get<float>());
        } else if(std::holds_alternative<bool>(value)) {
            return get<bool>() ? "1" : "0";
        } else if(std::holds_alternative<Nil>(value)) {
            return "nil";
        }
        return "";
    }
};

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

class Token {
public:
    TokenType type;
    std::string lexme;
    std::optional<Literal> literal;
    int line;
    Token(TokenType type_, std::string lexme_, int line_, std::optional<Literal> literal_);
    Token(TokenType type_, std::string lexme_, int line_);
    std::string toString() const; 
};

class RuntimeError : public std::runtime_error {
public:
    Token token;
    RuntimeError(Token token, const std::string& message) : 
        token { token }, std::runtime_error(message) {}
    ~RuntimeError() = default;
};

// Global use -----------------------------------------------------------------
bool hadError { false };
bool hadRuntimeError { false };
void runFile(std::string path);
void runPrompt();
void run(std::string source);
void error(Token token, string message);
void error(int line, string message);
void report(Token token, string where, string message);
void report(int line, string where, string message);
void runtimeError(RuntimeError error);

string substring(string str, int start, int end);
float strToFloat(string str);


// Token ----------------------------------------------------------------------
std::string tokenTypeToString(TokenType type);

Token::Token(TokenType type_, std::string lexme_, int line_, std::optional<Literal> literal_) :
    type {type_}, lexme {lexme_}, line {line_}, literal {literal_}
{}
Token::Token(TokenType type_, std::string lexme_, int line_) :
    Token(type_, lexme_, line_, std::nullopt) 
{}

std::string Token::toString() const {
    string literalStr = literal.has_value() ? literal->toString() : "";
    return tokenTypeToString(type) + " " + lexme + " " + literalStr;
}
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
class Expr;
class Binop;
class Unop;
class Grouping;
class LiteralExpr;
class Stmt;
class ExprStmt;
class PrintStmt;

// I'm using std::any instead of templates because templates don't work with
// virtual functions:(
class Visitor {
public:
    virtual Value visitBinop(const Binop* expr) const = 0;
    virtual Value visitUnop(const Unop* expr) const = 0;
    virtual Value visitGrouping(const Grouping* expr) const = 0;
    virtual Value visitLiteralExpr(const LiteralExpr* expr) const = 0;
    virtual Value visitExprStmt(const ExprStmt* exprstmt) const = 0;
    virtual Value visitPrintStmt(const PrintStmt* exprstmt) const = 0;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual Value accept(const Visitor* visitor) const = 0;
};

class ExprStmt : public Stmt {
public:
    std::shared_ptr<Expr> expr;
    ExprStmt(std::shared_ptr<Expr> expr) : expr {std::move(expr)} {}
    virtual Value accept(const Visitor* visitor) const override {
        if(!visitor) return Nil();
        return visitor->visitExprStmt(this);
    }
};

class PrintStmt : public Stmt {
public:
    std::shared_ptr<Expr> expr;
    PrintStmt(std::shared_ptr<Expr> expr) : expr {std::move(expr)} {}
    virtual Value accept(const Visitor* visitor) const override {
        if(!visitor) return Nil();
        return visitor->visitPrintStmt(this);
    }
};

class Expr {
public:
    virtual Value accept(const Visitor* visitor) const = 0;
    virtual ~Expr() = default;
};


/*
expression     → equality ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")" ;

program        → statement* EOF ;

statement      → exprStmt
               | printStmt ;

exprStmt       → expression ";" ;
printStmt      → "print" expression ";" ;
 */

class Binop : public Expr {
public:
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    Token op;
    Binop(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left { std::move(left) }, right { std::move(right) }, op { op }
    { }
    virtual ~Binop() override { }
    virtual Value accept(const Visitor* visitor) const override {
        if(!visitor) return Nil();
        return visitor->visitBinop(this);
    }
};

class Unop : public Expr {
public:
    std::shared_ptr<Expr> expr;
    Token op;
    Unop(Token op, std::shared_ptr<Expr> expr)
        : expr { std::move(expr) }, op { op }
    { }
    virtual ~Unop() override { }
    virtual Value accept(const Visitor* visitor) const override {
        if(!visitor) return Nil();
        return visitor->visitUnop(this);
    }
};

class Grouping : public Expr {
public:
    std::shared_ptr<Expr> expr;
    Grouping(std::shared_ptr<Expr> expr) : expr { std::move(expr) } {}
    virtual ~Grouping() override { }
    virtual Value accept(const Visitor* visitor) const override {
        if(!visitor) return Nil();
        return visitor->visitGrouping(this);
    }
};

class LiteralExpr : public Expr {
public:
    std::shared_ptr<Literal> value;
    LiteralExpr(std::shared_ptr<Literal> value) : value { std::move(value) } {}
    virtual ~LiteralExpr() override { }
    virtual Value accept(const Visitor* visitor) const override {
        if(!visitor) return Nil();
        return visitor->visitLiteralExpr(this);
    }
};

class AstPrinter : public Visitor {
public:
    string print(std::shared_ptr<Expr> expr) const {
        std::any result = expr->accept(this);
        if(result.type() == typeid(std::string)) {
            return std::any_cast<string>(result);
        } else {
            throw std::runtime_error("Unexpected return type from accept \
                    in AstPrinter!!!");
        }
    }
    virtual Value visitBinop(const Binop* binop) const override {
        return parenthesize(binop->op.lexme, binop->left, binop->right);
    }
    virtual Value visitUnop(const Unop* unop) const override {
        return parenthesize(unop->op.lexme, unop->expr);        
    }
    virtual Value visitGrouping(const Grouping* expr) const override {
        return parenthesize("group", expr->expr);
    }
    virtual Value visitLiteralExpr(const LiteralExpr* expr) const override {
        if(!expr->value) return "nil";
        return expr->value->toString();
    }

    template <typename... Exprs>
    void processExprsHelper(std::ostringstream& oss, 
            const std::shared_ptr<Exprs>&... exprs) const {
        ((oss << " " << std::any_cast<std::string>(exprs->accept(this))), ...);
    }
    template <typename... Exprs>
    string parenthesize(const string& name, 
            const std::shared_ptr<Exprs>&... exprs) const {
        std::ostringstream oss;
        oss << "(" << name;
        processExprsHelper(oss, exprs...);
        oss << ")";
        return oss.str();
    }
};

// Parser ---------------------------------------------------------------------

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
    ~ParseError() = default;
};

class Parser {
    std::vector<Token> tokens;
    int current;

    std::unique_ptr<Expr> expression() {
        return equality();
    }

    std::unique_ptr<Expr> equality() {
        auto expr = comparison();
        while(match(BANG_EQUAL, EQUAL_EQUAL)) {
            Token op = previous();
            auto right = comparison();
            expr = std::make_unique<Binop>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> comparison() {
        auto expr = term();
        while(match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
            Token op = previous();
            auto right = comparison();
            expr = std::make_unique<Binop>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> term() {
        auto expr = factor();
        while(match(MINUS, PLUS)) {
            Token op = previous();
            auto right = factor();
            expr = std::make_unique<Binop>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expr> factor() {
        auto expr = unary();
        while(match(SLASH, STAR)) {
            Token op = previous();
            auto right = unary();
            expr = std::make_unique<Binop>(std::move(expr), op, std::move(right));
        }
        return expr;
    }
    
    std::unique_ptr<Expr> unary() {
        if(match(BANG, MINUS)) {
            Token op = previous();
            auto right = unary();
            return std::make_unique<Unop>(op, std::move(right));
        }
        return primary();
    }

    std::unique_ptr<Expr> primary() {
        auto makeLit = []<typename T>(T t){
            return std::make_unique<LiteralExpr>(
                    std::make_shared<Literal>(t));
        };
        if(match(FALSE)) return makeLit(false);
        if(match(TRUE)) return makeLit(true);
        if(match(NIL)) return makeLit(Nil());
        
        if(match(NUMBER, STRING)) {
            auto literal = previous().literal;
            if(literal.has_value()) {
                return makeLit(literal.value());
            } else {
                throw std::runtime_error("[Primary] Literal value is empty");
            }
        }

        if(match(LEFT_PAREN)) {
            auto expr = expression();
            consume(RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_unique<Grouping>(std::move(expr));
        }

        throw parserError(peek(), "Expected expression.");
    }

    template <typename... TokenTypes>
    bool match(TokenTypes... types) {
        for(const auto& type: {types...}) {
            if(check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    Token consume(TokenType type, string message) {
        if(check(type)) return advance();

        throw parserError(peek(), message);
    }

    bool check(TokenType type) {
        if(isAtEnd()) return false;
        return peek().type == type;
    }

    Token advance() {
        if(!isAtEnd()) current++;
        return previous();
    }

    bool isAtEnd() {
        return peek().type == EOF_;
    }

    Token peek() {
        return tokens[current];
    }

    Token previous() {
        return tokens[current - 1];
    }

    ParseError parserError(Token token, string message) {
        error(token, message);
        return ParseError(message);
    }

    void synchronize() {
        advance();
        while(!isAtEnd()) {
            if(previous().type == SEMICOLON) return;

            switch(peek().type) {
                case CLASS:
                case FUN:
                case VAR:
                case FOR:
                case IF:
                case WHILE:
                case PRINT:
                case RETURN:
                  return;
                default: break;
            }
            advance();
        }
    }

    std::shared_ptr<Stmt> printStatement() {
        auto expr = expression();
        consume(SEMICOLON, "Expected ';' after a value.");
        return std::make_shared<PrintStmt>(std::move(expr));
    }

    std::shared_ptr<Stmt> expressionStatement() {
        auto expr = expression();
        consume(SEMICOLON, "Expected ';' after expression.");
        return std::make_shared<ExprStmt>(std::move(expr));
    }

    std::shared_ptr<Stmt> statement() {
        if(match(PRINT)) return printStatement();
        return expressionStatement();
    }

public:
    Parser(std::vector<Token> tokens) :
        tokens { tokens }, current { 0 } {}

    std::vector<std::shared_ptr<Stmt>> parse() {
        std::vector<std::shared_ptr<Stmt>> statements;
        while(!isAtEnd()) {
            statements.push_back(statement());
        }
        return statements;
    }
};

// Interpreter ----------------------------------------------------------------


class Interpreter : public Visitor {
    void checkNumberOperand(Token op, Value& val) const {
        if(std::holds_alternative<float>(val)) return;
        throw RuntimeError(op, "Operand must be a number.");
    }
    void checkNumberOperands(Token op, Value& left, Value& right) const {
        if(std::holds_alternative<float>(left) &&
           std::holds_alternative<float>(right)) return;
        throw RuntimeError(op, "Operands must be a number.");
    }
    void reportDifferentTypesOperands() const {
        //throw RuntimeError(
    }
    bool isTruthy(Value val) const {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Nil>) {
                return false;  // nil is false
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg;  // return the boolean value itself
            } else if constexpr (std::is_same_v<T, float>) {
                return arg != 0;  // nonzero numbers are true
            } else if constexpr (std::is_same_v<T, std::string>) {
                return !arg.empty();  // non-empty strings are true
            }
            return false; // Default case (shouldn't happen)
        }, val);
    }
    bool isEqual(Value left, Value right) const {
        if(std::holds_alternative<Nil>(left)) {
           return std::holds_alternative<Nil>(right);
        }
        auto eq = [&left, &right]<typename T>() -> bool {
            auto l = std::get<T>(left);
            auto r = std::get<T>(right);
            return l == r;
        };
        if(std::holds_alternative<string>(left) 
        && std::holds_alternative<string>(right)) 
            return eq.template operator()<string>();
        if(std::holds_alternative<float>(left)
        && std::holds_alternative<float>(right)) 
            return eq.template operator()<float>();
        if(std::holds_alternative<bool>(left)
        && std::holds_alternative<bool>(right)) 
            return eq.template operator()<bool>();
        //reportDifferentTypesOperands();
        // for now two different types are always not equal
        return false;
    }
public:
    virtual Value visitBinop(const Binop* expr) const override {
        Value left = evaluate(&*expr->left);
        Value right = evaluate(&*expr->right);
        auto executeBinop = 
            [&left, &right]<typename T, typename F>(F fn) -> T { 
            auto l = std::get<T>(left);
            auto r = std::get<T>(right);
            return fn(l, r);
        };
        switch (expr->op.type) {
            case EQUAL_EQUAL: return isEqual(left, right);
            case BANG_EQUAL: return !isEqual(left, right);
            case GREATER:
                checkNumberOperands(expr->op, left, right);
                return executeBinop.template operator()<float>(std::greater<float>{});
            case GREATER_EQUAL:
                checkNumberOperands(expr->op, left, right);
                return executeBinop.template operator()<float>(std::greater_equal<float>{});
            case LESS:
                checkNumberOperands(expr->op, left, right);
                return executeBinop.template operator()<float>(std::less<float>{});
            case LESS_EQUAL:
                checkNumberOperands(expr->op, left, right);
                return executeBinop.template operator()<float>(std::less_equal<float>{});
            case MINUS:
                checkNumberOperands(expr->op, left, right);
                return executeBinop.template operator()<float>(std::minus<float>{});
            case SLASH:
                checkNumberOperands(expr->op, left, right);
                return executeBinop.template operator()<float>(std::divides<float>{});
            case STAR:
                checkNumberOperands(expr->op, left, right);
                return executeBinop.template operator()<float>(std::multiplies<float>{});
            case PLUS:
                if(std::holds_alternative<float>(left) 
                && std::holds_alternative<float>(right)) {
                    return executeBinop.template operator()<float>
                        (std::plus<float>{});
                }
                
                if(std::holds_alternative<string>(left) 
                && std::holds_alternative<string>(right)) {
                    auto l = std::get<string>(left);
                    auto r = std::get<string>(right);
                    return l + r;
                }
                throw RuntimeError(expr->op, "Operands must be two numbers or two strings");
            default: break;
        }
        return Nil();
    }
    virtual Value visitUnop(const Unop* expr) const override {
        Value right = evaluate(&*expr->expr);
        switch(expr->op.type) {
            case BANG: {
                return !isTruthy(right);
            }
            case MINUS: {
                checkNumberOperand(expr->op, right);
                float val = std::get<float>(right);
                return -val;
            }
            default: break;
        }
        return Nil();
    }
    virtual Value visitGrouping(const Grouping* expr) const override {
        return evaluate(expr);
    }
    virtual Value visitLiteralExpr(const LiteralExpr* expr) const override {
        return expr->value->value;
    }
    virtual Value visitExprStmt(const ExprStmt* exprstmt) const override {
        evaluate(&*exprstmt->expr);
        return Nil();
    }
    virtual Value visitPrintStmt(const PrintStmt* exprstmt) const override {
        Value res = evaluate(&*exprstmt->expr);
        std::cout << valueToString(res) << std::endl;
        return Nil();
    }
    Value evaluate(const Expr* expr) const {
        return expr->accept(this);
    }
    void execute(const std::shared_ptr<Stmt>& stmt) {
        stmt->accept(this);
    }
    void interpret(const std::vector<std::shared_ptr<Stmt>>& program) {
        try {
            for(const auto& stmt: program) {
                execute(stmt);
            }
        } catch(RuntimeError error) {
            runtimeError(error); 
        }
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



void runFile(std::string path) {
    std::ifstream input(path);
    if(!input.good()) {
        std::cerr << "runFile(path) error\nFailed to open " << path << std::endl;
        return;
    }
    std::stringstream buffer;
    buffer << input.rdbuf();
    run(buffer.str());
    if(hadError) exit(65);
    if(hadRuntimeError) exit(70);
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

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> program = parser.parse();
    // Stop if there was a syntax error 
    if(hadError) return;

    Interpreter interpreter; 
    interpreter.interpret(program);
}

void error(Token token, string message) {
    if (token.type == EOF_) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexme + "'", message);
    }
}
void error(int line, string message) {
    report(line, "", message);
}

void report(int line, string where, string message) {
    cerr << "[line " << line << "] Error " << where << ": " << message << endl;
    hadError = true;
}

void report(Token token, string where, string message) {
    cerr << "[line " << token.line << "] Error " << where << ": " << message << endl;
    hadError = true;
}

void runtimeError(RuntimeError error) {
    std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
    hadRuntimeError = true;
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
