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
        float f = std::get<float>(value);
        int c;
        if(std::abs(f - int(f)) == 0.0) {
            int c = f;
            return std::to_string(c);
        }
        return std::to_string(f);
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
    BREAK,

    EOF_
};

class Token {
public:
    TokenType type;
    std::string lexeme;
    std::optional<Literal> literal;
    int line;
    Token(TokenType type_, std::string lexeme_, int line_, std::optional<Literal> literal_);
    Token(TokenType type_, std::string lexeme_, int line_);
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

Token::Token(TokenType type_, std::string lexeme_, int line_, std::optional<Literal> literal_) :
    type {type_}, lexeme {lexeme_}, line {line_}, literal {literal_}
{}
Token::Token(TokenType type_, std::string lexeme_, int line_) :
    Token(type_, lexeme_, line_, std::nullopt) 
{}

std::string Token::toString() const {
    string literalStr = literal.has_value() ? literal->toString() : "";
    return tokenTypeToString(type) + " " + lexeme + " " + literalStr;
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
class VariableExpr;
class Assign;
class Logical;

class Stmt;
class ExprStmt;
class PrintStmt;
class VarStmt;
class BlockStmt;
class IfStmt;
class WhileStmt;
class BreakStmt;

// I'm using std::any instead of templates because templates don't work with
// virtual functions:(
class Visitor {
public:
    virtual Value visitBinop(Binop* expr) = 0;
    virtual Value visitUnop(Unop* expr) = 0;
    virtual Value visitGrouping(Grouping* expr) = 0;
    virtual Value visitLiteralExpr(LiteralExpr* expr) = 0;
    virtual Value visitAssign(Assign* expr) = 0;
    virtual Value visitVariableExpr(VariableExpr* expr) = 0;
    virtual Value visitLogical(Logical* expr) = 0;

    virtual Value visitExprStmt(ExprStmt* exprstmt) = 0;
    virtual Value visitPrintStmt(PrintStmt* print) = 0;
    virtual Value visitVarStmt(VarStmt* var) = 0;
    virtual Value visitBlockStmt(BlockStmt* block) = 0;
    virtual Value visitIfStmt(IfStmt* stmt) = 0;
    virtual Value visitWhileStmt(WhileStmt* stmt) = 0;
    virtual Value visitBreakStmt(BreakStmt* stmt) = 0;
};


class Stmt {
public:
    virtual ~Stmt() = default;
    virtual Value accept(Visitor* visitor) = 0;
};

class BreakStmt : public Stmt {
public:
    BreakStmt() {}
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitBreakStmt(this);
    }
};

class WhileStmt : public Stmt {
public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition{ std::move(condition) }, body{ std::move(body) } {}
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitWhileStmt(this);
    }
};

class IfStmt : public Stmt {
public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
    IfStmt(std::shared_ptr<Expr>& condition, 
        std::shared_ptr<Stmt>& thenBranch,
        std::shared_ptr<Stmt>& elseBranch)
        : condition(std::move(condition)),
        thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}
    virtual Value accept(Visitor* visitor) override {
        return visitor->visitIfStmt(this);
    }
};
    

class BlockStmt : public Stmt {
public:
    std::vector<std::shared_ptr<Stmt>> statements;
    BlockStmt() : statements {} {}
    BlockStmt(const std::vector<std::shared_ptr<Stmt>>& otherStatements)
        : statements(otherStatements) {}
    BlockStmt(const BlockStmt& other) { BlockStmt(other.statements); }
    template <typename... Stmts>
    BlockStmt(const std::shared_ptr<Stmts>&... stmts) : BlockStmt() {
        ((statements.push_back(std::move(stmts))), ...);
    }
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitBlockStmt(this);
    }
};

class ExprStmt : public Stmt {
public:
    std::shared_ptr<Expr> expr;
    ExprStmt(std::shared_ptr<Expr> expr) : expr {std::move(expr)} {}
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitExprStmt(this);
    }
};

class PrintStmt : public Stmt {
public:
    std::shared_ptr<Expr> expr;
    PrintStmt(std::shared_ptr<Expr> expr) : expr {std::move(expr)} {}
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitPrintStmt(this);
    }
};

class VarStmt : public Stmt {
public:
    std::shared_ptr<Expr> initializer;
    Token name;
    VarStmt(std::shared_ptr<Expr> expr, Token name) :
        initializer {std::move(expr)}, name { name } {}
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitVarStmt(this);
    }
};


class Expr {
public:
    virtual Value accept(Visitor* visitor) = 0;
    virtual ~Expr() = default;
};

class Logical : public Expr {
public:
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
    Logical(std::shared_ptr<Expr> left,
            Token op,
            std::shared_ptr<Expr> right)
        : left { std::move(left) }, op { op }, right { std::move(right) }
    {}
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitLogical(this);
    }
};

class Assign : public Expr {
public:
    Token name;
    std::shared_ptr<Expr> value;
    Assign(Token name, std::shared_ptr<Expr> expr)
        : name { name }, value { std::move(expr) } { }
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitAssign(this);
    }
};

class VariableExpr : public Expr {
public:
    Token name;
    VariableExpr(Token name) : name { name } {}
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitVariableExpr(this);
    }
};

class Binop : public Expr {
public:
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    Token op;
    Binop(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left { std::move(left) }, right { std::move(right) }, op { op }
    { }
    virtual Value accept(Visitor* visitor) override {
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
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitUnop(this);
    }
};

class Grouping : public Expr {
public:
    std::shared_ptr<Expr> expr;
    Grouping(std::shared_ptr<Expr> expr) : expr { std::move(expr) } {}
    virtual ~Grouping() override { }
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitGrouping(this);
    }
};

class LiteralExpr : public Expr {
public:
    std::shared_ptr<Literal> value;
    LiteralExpr(std::shared_ptr<Literal> value) : value { std::move(value) } {}
    LiteralExpr(bool b) : value { std::make_shared<Literal>(b) } {}
    LiteralExpr(float f) : value { std::make_shared<Literal>(f) } {}
    LiteralExpr(string s) : value { std::make_shared<Literal>(s) } {}
    LiteralExpr(Nil s) : value { std::make_shared<Literal>(s) } {}
    virtual ~LiteralExpr() override { }
    virtual Value accept(Visitor* visitor) override {
        if(!visitor) return Nil();
        return visitor->visitLiteralExpr(this);
    }
};

class AstPrinter : public Visitor {
public:
    string print(std::shared_ptr<Expr> expr) {
        std::any result = expr->accept(this);
        if(result.type() == typeid(std::string)) {
            return std::any_cast<string>(result);
        } else {
            throw std::runtime_error("Unexpected return type from accept \
                    in AstPrinter!!!");
        }
    }
    virtual Value visitBinop(Binop* binop) override {
        return parenthesize(binop->op.lexeme, binop->left, binop->right);
    }
    virtual Value visitUnop(Unop* unop) override {
        return parenthesize(unop->op.lexeme, unop->expr);        
    }
    virtual Value visitGrouping(Grouping* expr) override {
        return parenthesize("group", expr->expr);
    }
    virtual Value visitLiteralExpr(LiteralExpr* expr) override {
        if(!expr->value) return "nil";
        return expr->value->toString();
    }

    template <typename... Exprs>
    void processExprsHelper(std::ostringstream& oss, 
            const std::shared_ptr<Exprs>&... exprs) {
        ((oss << " " << std::any_cast<std::string>(exprs->accept(this))), ...);
    }
    template <typename... Exprs>
    string parenthesize(const string& name, 
            const std::shared_ptr<Exprs>&... exprs) {
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

    std::shared_ptr<Expr> expression() {
        //return equality();
        return assignment();
    }

    std::shared_ptr<Expr> orExpr() {
        std::shared_ptr<Expr> expr = andExpr();
        while(match(OR)) {
            Token op = previous();
            std::shared_ptr<Expr> right = andExpr();
            expr = std::make_shared<Logical>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::shared_ptr<Expr> andExpr() {
        std::shared_ptr<Expr> expr = equality();
        while(match(AND)) {
            Token op = previous();
            std::shared_ptr<Expr> right = equality();
            expr = std::make_shared<Logical>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::shared_ptr<Expr> assignment() {
        std::shared_ptr<Expr> expr = orExpr();

        if(match(EQUAL)) {
            Token equals = previous();
            std::shared_ptr<Expr> value = assignment();

            if(VariableExpr* v = dynamic_cast<VariableExpr*>(&*expr)) {
                Token name = v->name;
                return std::make_shared<Assign>(name, value);
            }
            error(equals, "Invalid assignment target");
        }

        return expr;
    }

    std::shared_ptr<Expr> equality() {
        auto expr = comparison();
        while(match(BANG_EQUAL, EQUAL_EQUAL)) {
            Token op = previous();
            auto right = comparison();
            expr = std::make_unique<Binop>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::shared_ptr<Expr> comparison() {
        auto expr = term();
        while(match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
            Token op = previous();
            auto right = comparison();
            expr = std::make_unique<Binop>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::shared_ptr<Expr> term() {
        auto expr = factor();
        while(match(MINUS, PLUS)) {
            Token op = previous();
            auto right = factor();
            expr = std::make_unique<Binop>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::shared_ptr<Expr> factor() {
        auto expr = unary();
        while(match(SLASH, STAR)) {
            Token op = previous();
            auto right = unary();
            expr = std::make_unique<Binop>(std::move(expr), op, std::move(right));
        }
        return expr;
    }
    
    std::shared_ptr<Expr> unary() {
        if(match(BANG, MINUS)) {
            Token op = previous();
            auto right = unary();
            return std::make_unique<Unop>(op, std::move(right));
        }
        return primary();
    }

    std::shared_ptr<Expr> primary() {
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

        if(match(IDENTIFIER)) {
            return std::make_unique<VariableExpr>(previous());
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

    std::shared_ptr<Stmt> whileStatement() {
        consume(LEFT_PAREN, "Expect '(' after 'while'.");
        std::shared_ptr<Expr> cond = expression();
        consume(RIGHT_PAREN, "Expect ')' after while condition.");
        std::shared_ptr<Stmt> body = statement(true);
        return std::make_shared<WhileStmt>(std::move(cond), std::move(body));
    }

    std::shared_ptr<Stmt> forStatement() {
        consume(LEFT_PAREN, "Expect '(' after 'for'.");
        std::shared_ptr<Stmt> initializer; 
        if(match(SEMICOLON)) {
            initializer = nullptr;
        } else if(match(VAR)) {
            initializer = varDeclaration();
        } else {
            initializer = expressionStatement();
        }

        std::shared_ptr<Expr> condition { nullptr };
        if(!check(SEMICOLON)) {
            condition = expression();
        }
        consume(SEMICOLON, "Expect ';' after loop condition.");

        std::shared_ptr<Expr> increment { nullptr };
        if(!check(RIGHT_PAREN)) {
            increment = expression();
        }
        consume(RIGHT_PAREN, "Expect ';' after for clauses");

        std::shared_ptr<Stmt> body = statement(true);

        if(increment) {
            body = std::make_shared<BlockStmt>(
                        body,
                        std::make_shared<ExprStmt>(increment)
                    );
        }

        if(!condition) condition = std::make_shared<LiteralExpr>(true);
        body = std::make_shared<WhileStmt>(std::move(condition), std::move(body));

        if(initializer) {
            body = std::make_shared<BlockStmt>(
                    initializer,
                    body
                    );
        }

        return body;
    }

    std::shared_ptr<Stmt> statement(bool inLoop=false) {
        if(match(FOR)) return forStatement();
        if(match(IF)) return ifStatement(inLoop);
        if(match(PRINT)) return printStatement();
        if(match(WHILE)) return whileStatement();
        if(match(LEFT_BRACE)) return std::make_shared<BlockStmt>(block(inLoop));
        if(match(BREAK)) return breakStatement(inLoop);
        return expressionStatement();
    }

    std::shared_ptr<Stmt> breakStatement(bool inLoop) {
        if(!inLoop) parserError(previous(), "'break' statement outside of a loop.");
        consume(SEMICOLON, "Expect ';' after 'break'");
        return std::make_shared<BreakStmt>();
    }

    std::shared_ptr<Stmt> ifStatement(bool inLoop) {
        consume(LEFT_PAREN, "Expect '(' after 'if'.");
        std::shared_ptr<Expr> condition = expression();
        consume(RIGHT_PAREN, "Expect ')' after if condition.");

        std::shared_ptr<Stmt> thenBranch { statement(inLoop) };
        std::shared_ptr<Stmt> elseBranch { nullptr };
        if(match(ELSE)) {
            elseBranch = statement(inLoop);
        }
        auto ifstmt = std::make_shared<IfStmt>(
                condition, thenBranch, elseBranch);
        return ifstmt;
    }

    std::vector<std::shared_ptr<Stmt>> block(bool inLoop) {
        std::vector<std::shared_ptr<Stmt>> statements;
        while(!check(RIGHT_BRACE) && !isAtEnd()) {
            statements.push_back(declaration(inLoop));
        }
        consume(RIGHT_BRACE, "Expect '}' after end of a block");
        return statements;
    }
    
    std::shared_ptr<Stmt> declaration(bool inLoop=false) {
        try {
            if(match(VAR)) return varDeclaration();
            return statement(inLoop);
        } catch(ParseError error) {
            synchronize();
            return nullptr;
        }
    }

    std::shared_ptr<Stmt> varDeclaration() {
        Token name = consume(IDENTIFIER, "Expected a variable name.");
        std::shared_ptr<Expr> initializer = nullptr;
        if(match(EQUAL)) {
            initializer = expression();
        }
        consume(SEMICOLON, "Expected ';' after variable declaration.");
        return std::make_shared<VarStmt>(std::move(initializer), name);
    }

public:
    Parser(std::vector<Token> tokens) :
        tokens { tokens }, current { 0 } {}

    std::vector<std::shared_ptr<Stmt>> parse() {
        std::vector<std::shared_ptr<Stmt>> statements;
        while(!isAtEnd()) {
            statements.push_back(declaration());
        }
        return statements;
    }
};

// Interpreter ----------------------------------------------------------------
class Environment {
    std::map<string, Value> values;
    std::shared_ptr<Environment> enclosing;
public:
    Environment(std::shared_ptr<Environment> enclosing) 
        : values {}, enclosing(std::move(enclosing)) {}
    Environment() : enclosing(nullptr) { }
    void define(string name, Value value) {
        values[name] = value;
    }
    Value get(const Token& name) const {
        if(values.contains(name.lexeme)) {
            return values.at(name.lexeme);
        }
        if(enclosing) return enclosing->get(name);
        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'."); 
    }
    void assign(Token name, Value value) {
        if(values.contains(name.lexeme)) {
            values[name.lexeme] = value;
            return;
        }
        if(enclosing) {
            enclosing->assign(name, value);
            return;
        }
        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }
};

class BreakLoop : public std::runtime_error {
public:
    BreakLoop() : std::runtime_error("") {}
    ~BreakLoop() = default;
};

class Interpreter : public Visitor {
    std::shared_ptr<Environment> environment;
    bool loopBreak { false };

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
    void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements,
            std::shared_ptr<Environment> env) {
        std::shared_ptr<Environment> prevEnv = std::move(this->environment);
        this->environment = env;

        for(const auto& stmt: statements) {
            execute(stmt);
        }

        this->environment = prevEnv;
    }
public:
    virtual Value visitBinop(Binop* expr) override {
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
    virtual Value visitUnop(Unop* expr) override {
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
    virtual Value visitGrouping(Grouping* expr) override {
        return evaluate(expr);
    }
    virtual Value visitLiteralExpr(LiteralExpr* expr) override {
        return expr->value->value;
    }
    virtual Value visitExprStmt(ExprStmt* exprstmt) override {
        evaluate(&*exprstmt->expr);
        return Nil();
    }
    virtual Value visitPrintStmt(PrintStmt* varstmt) override {
        Value res = evaluate(&*varstmt->expr);
        std::cout << valueToString(res) << std::endl;
        return Nil();
    }
    virtual Value visitVarStmt(VarStmt* stmt) override {
        Value val = Nil();
        if(stmt->initializer) {
            val = evaluate(&*stmt->initializer);
        }
        environment->define(stmt->name.lexeme, val);
        return val;
    }
    virtual Value visitVariableExpr(VariableExpr* var) override {
        return environment->get(var->name);
    }
    virtual Value visitAssign(Assign* expr) override {
        Value value = evaluate(&*expr->value);
        environment->assign(expr->name, value);
        return value;
    }
    virtual Value visitBlockStmt(BlockStmt* stmt) override {
        executeBlock(stmt->statements, std::make_shared<Environment>(environment));
        return Nil();
    }
    virtual Value visitIfStmt(IfStmt* stmt) override {
        if(isTruthy(evaluate(&*stmt->condition))) {
            execute(stmt->thenBranch);
        } else if(stmt->elseBranch) {
            execute(stmt->elseBranch);
        }
        return Nil();
    }
    virtual Value visitLogical(Logical* expr) override {
        Value left = evaluate(&*expr->left);
        if(expr->op.type == OR) {
            if(isTruthy(left)) return left;
        } else if(expr->op.type == AND) {
            if(!isTruthy(left)) return left;
        }
        return evaluate(&*expr->right);
    }
    virtual Value visitWhileStmt(WhileStmt* stmt) override {
        while(isTruthy(evaluate(&*stmt->condition))) {
            try {
                execute(stmt->body);
            } catch(BreakLoop e) {
                break;
            }
        }
        return Nil();
    }
    virtual Value visitBreakStmt(BreakStmt* stmt) override {
        throw BreakLoop();
    }

    Interpreter() : environment { std::make_shared<Environment>() } {}

    Value evaluate(Expr* expr) {
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
        report(token.line, " at '" + token.lexeme + "'", message);
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
    keywords["break"] = BREAK;
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
