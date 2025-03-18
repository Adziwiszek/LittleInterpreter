#include "../include/Parser.hpp"
#include "../include/Types.hpp"
//#include "../include/Expr.hpp"
//#include "../include/Stmt.hpp"

ExprPtr Parser::expression() {
  return assignment();
}

ExprPtr Parser::orExpr() {
  ExprPtr expr = andExpr();
  while(match(OR)) {
    Token op = previous();
    ExprPtr right = andExpr();
    expr = std::make_shared<Expr::Logical>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::andExpr() {
  ExprPtr expr = equality();
  while(match(AND)) {
    Token op = previous();
    ExprPtr right = equality();
    expr = std::make_shared<Expr::Logical>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::assignment() {
  ExprPtr expr = orExpr();

  if(match(EQUAL)) {
    Token equals = previous();
    ExprPtr value = assignment();

    if(Expr::Variable* v = dynamic_cast<Expr::Variable*>(&*expr)) {
      Token name = v->name;
      return std::make_shared<Expr::Assign>(name, value);
    }
    lox->error(equals, "Invalid assignment target");
  }

  return expr;
}

ExprPtr Parser::equality() {
  auto expr = comparison();
  while(match(BANG_EQUAL, EQUAL_EQUAL)) {
    Token op = previous();
    auto right = comparison();
    expr = std::make_unique<Expr::Binop>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::comparison() {
  auto expr = term();
  while(match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
    Token op = previous();
    auto right = comparison();
    expr = std::make_unique<Expr::Binop>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::term() {
  auto expr = factor();
  while(match(MINUS, PLUS)) {
    Token op = previous();
    auto right = factor();
    expr = std::make_unique<Expr::Binop>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::factor() {
  auto expr = unary();
  while(match(SLASH, STAR)) {
    Token op = previous();
    auto right = unary();
    expr = std::make_unique<Expr::Binop>(std::move(expr), op, std::move(right));
  }
  return expr;
}

ExprPtr Parser::unary() {
  if(match(BANG, MINUS, PLUSPLUS, MINUSMINUS)) {
    Token op = previous();
    auto right = unary();
    return std::make_unique<Expr::Unop>(op, std::move(right));
  }
  return call();
}

ExprPtr Parser::call() {
  ExprPtr expr = primary();
  while(true) {
    if(match(LEFT_PAREN)) {
      expr = finishCall(expr);
    } else break;
  }
  return expr;
}

ExprPtr Parser::finishCall(ExprPtr callee) {
  std::vector<ExprPtr> args {};
  if(!check(RIGHT_PAREN)) {
    do {
      if (args.size() >= 255) {
        lox->error(peek(), "Can't have more than 255 arguments.");
      }
      args.push_back(expression());
    } while(match(COMMA));
  }
  Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");
  return std::make_shared<Expr::Call>(callee, paren, args);
}

ExprPtr Parser::primary() {
  auto makeLit = []<typename T>(T t){
    return std::make_unique<Expr::Literal>(
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
    return std::make_unique<Expr::Variable>(previous());
  }

  if(match(LEFT_PAREN)) {
    auto expr = expression();
    //AstPrinter::print_(expr);

    consume(RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<Expr::Grouping>(std::move(expr));
  }

  throw parserError(peek(), "Expected expression.");
}

template <typename... TokenTypes>
bool Parser::match(TokenTypes... types) {
  for(const auto& type: {types...}) {
    if(check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

Token Parser::consume(TokenType type, std::string message) {
  if(check(type)) return advance();

  throw parserError(peek(), message);
}

bool Parser::check(TokenType type) {
  if(isAtEnd()) return false;
  return peek().type == type;
}

Token Parser::advance() {
  if(!isAtEnd()) current++;
  return previous();
}

bool Parser::isAtEnd() {
  return peek().type == EOF_;
}

Token Parser::peek() {
  return tokens[current];
}

Token Parser::previous() {
  return tokens[current - 1];
}

ParseError Parser::parserError(Token token, std::string message) {
  lox->error(token, message);
  return ParseError(message);
}

void Parser::synchronize() {
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

StmtPtr Parser::printStatement() {
  auto expr = expression();
  consume(SEMICOLON, "Expected ';' after a value.");
  return std::make_shared<Stmt::Print>(std::move(expr));
}

StmtPtr Parser::expressionStatement() {
  auto expr = expression();
  consume(SEMICOLON, "Expected ';' after expression.");
  return std::make_shared<Stmt::Expr>(std::move(expr));
}

StmtPtr Parser::whileStatement() {
  consume(LEFT_PAREN, "Expect '(' after 'while'.");
  ExprPtr cond = expression();
  consume(RIGHT_PAREN, "Expect ')' after while condition.");
  StmtPtr body = statement();
  return std::make_shared<Stmt::While>(std::move(cond), std::move(body));
}

StmtPtr Parser::forStatement() {
  consume(LEFT_PAREN, "Expect '(' after 'for'.");
  StmtPtr initializer; 
  if(match(SEMICOLON)) {
    initializer = nullptr;
  } else if(match(VAR)) {
    initializer = varDeclaration();
  } else {
    initializer = expressionStatement();
  }

  ExprPtr condition { nullptr };
  if(!check(SEMICOLON)) {
    condition = expression();
  }
  consume(SEMICOLON, "Expect ';' after loop condition.");

  ExprPtr increment { nullptr };
  if(!check(RIGHT_PAREN)) {
    increment = expression();
  }
  consume(RIGHT_PAREN, "Expect ';' after for clauses");

  StmtPtr body = statement();

  if(increment) {
    body = std::make_shared<Stmt::Block>(
        body,
        std::make_shared<Stmt::Expr>(increment)
        );
  }

  if(!condition) condition = std::make_shared<Expr::Literal>(true);
  body = std::make_shared<Stmt::While>(std::move(condition), std::move(body));

  if(initializer) {
    body = std::make_shared<Stmt::Block>(
        initializer,
        body
        );
  }

  return body;
}

StmtPtr Parser::returnStatement() {
  Token keyword = previous();
  ExprPtr value = nullptr;
  if(!check(SEMICOLON)) {
    value = expression();
  }
  consume(SEMICOLON, "Expect ';' after return value");
  return std::make_shared<Stmt::Return>(keyword, std::move(value));
}

StmtPtr Parser::statement() {
  if(match(FOR)) return forStatement();
  if(match(IF)) return ifStatement();
  if(match(PRINT)) return printStatement();
  if(match(RETURN)) return returnStatement();
  if(match(WHILE)) return whileStatement();
  if(match(LEFT_BRACE)) return std::make_shared<Stmt::Block>(block());
  if(match(BREAK)) return breakStatement();
  return expressionStatement();
}

StmtPtr Parser::breakStatement() {
  Token keyword = previous();
  consume(SEMICOLON, "Expect ';' after 'break'");
  return std::make_shared<Stmt::Break>(keyword);
}

StmtPtr Parser::ifStatement() {
  consume(LEFT_PAREN, "Expect '(' after 'if'.");
  ExprPtr condition = expression();
  consume(RIGHT_PAREN, "Expect ')' after if condition.");

  StmtPtr thenBranch { statement() };
  StmtPtr elseBranch { nullptr };
  if(match(ELSE)) {
    elseBranch = statement();
  }
  auto ifstmt = std::make_shared<Stmt::If>(
      condition, thenBranch, elseBranch);
  return ifstmt;
}

std::vector<StmtPtr> Parser::block() {
  std::vector<StmtPtr> statements;
  while(!check(RIGHT_BRACE) && !isAtEnd()) {
    statements.push_back(declaration());
  }
  consume(RIGHT_BRACE, "Expect '}' after end of a block");
  return statements;
}

StmtPtr Parser::declaration() {
  try {
    if(match(FUN)) return function("function");
    if(match(VAR)) return varDeclaration();
    return statement();
  } catch(ParseError error) {
    synchronize();
    return nullptr;
  }
}

std::shared_ptr<Stmt::Function> Parser::function(std::string kind) {
  Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
  consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
  std::vector<Token> args {};
  if(!check(RIGHT_PAREN)) {
    do {
      if(args.size() >= 255) {
        lox->error(peek(), "Can't have more than 255 parameters.");
      }
      args.push_back(
          consume(IDENTIFIER, "Expect parameter name.")
      );
    } while(match(COMMA));
  }
  consume(RIGHT_PAREN, "Expect ')' after parameters");
  consume(LEFT_BRACE, "Expect '{' after function parameters");
  std::vector<StmtPtr> body = block();
  return std::make_shared<Stmt::Function>(name, args, body);
}

StmtPtr Parser::varDeclaration() {
  Token name = consume(IDENTIFIER, "Expected a variable name.");
  ExprPtr initializer = nullptr;
  if(match(EQUAL)) {
    initializer = expression();
  }
  consume(SEMICOLON, "Expected ';' after variable declaration.");
  return std::make_shared<Stmt::Var>(std::move(initializer), name);
}

Parser::Parser(std::vector<Token> tokens, std::shared_ptr<Lox> lox) :
  tokens { tokens }, current { 0 }, lox { std::move(lox) } {}

std::vector<StmtPtr> Parser::parse() {
  std::vector<StmtPtr> statements;
  while(!isAtEnd()) {
    statements.push_back(declaration());
  }
  return statements;
}
