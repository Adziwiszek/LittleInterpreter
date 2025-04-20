#include "../include/Scanner.hpp"

std::string substring(std::string str, int start, int end) {
  std::string res = "";
  for(int i = start; i < end; i++) {
    res += str[i];     
  }
  return res;
}

Scanner::Scanner(std::string source, Lox& lox)
  : tokens {}, source {source}, keywords {}, lox { lox } {
  setupKeywords(); 
}

std::vector<Token> Scanner::scanTokens() {
  while(!isAtEnd()) {
    start = current;
    scanToken();
  }
  tokens.push_back(Token(EOF_, "", line));
  return tokens;
}

void Scanner::scanToken() {
  char c = advance();
  switch (c) {
    case '(': addToken(LEFT_PAREN); break;
    case ')': addToken(RIGHT_PAREN); break;
    case '{': addToken(LEFT_BRACE); break;
    case '}': addToken(RIGHT_BRACE); break;
    case ',': addToken(COMMA); break;
    case '.': addToken(DOT); break;
    case '-': addToken(MINUS); break;
    case '+': 
              addToken(match('+') ? PLUSPLUS : PLUS); 
              break;
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
                lox.error(line, "Unexpected character!"); 
              }
              break;
  }
}

void Scanner::identifierLex() {
  while (isAlphaNumeric(peek())) advance();
  std::string text = substring(source, start, current);
  TokenType type = keywords.contains(text) ? keywords[text] : IDENTIFIER;
  addToken(type); 
}

void Scanner::numberLex() {
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

char Scanner::peekNext() {
  if(current + 1 >= source.length()) return '\0';
  return source[current + 1];
}

bool Scanner::isDigit(char c) {
  return c >= '0' && c <= '9';
}

bool Scanner::isAlphaNumeric(char c) {
  return isDigit(c) || isalpha(c);
}

char Scanner::peek() {
  if(isAtEnd()) return '\0';
  return source[current];
}

void Scanner::stringLex() {
  while(peek() != '"' && !isAtEnd()) {
    if(peek() == '\n') line++;
    advance();
  }
  if(isAtEnd()) {
    lox.error(line, "Undetermined string!");
  }
  advance();
  std::string finalStr = substring(source, start + 1, current - 1);
  std::optional<Literal> literalopt { std::optional<Literal>(finalStr) };
  addToken(STRING, literalopt);
}

bool Scanner::match(char expected) {
  if(isAtEnd()) return false;
  if(source[current] != expected) return false;
  current++;
  return true;
}

char Scanner::advance() {
  return source[current++];
}

void Scanner::addToken(TokenType type) {
  addToken(type, std::nullopt);
}

void Scanner::addToken(TokenType type, std::optional<Literal> literal) {
  std::string text = substring(source, start, current);
  tokens.push_back(Token(type, text, line, literal));
}

bool Scanner::isAtEnd() {
  return current >= source.length();
}

void Scanner::Scanner::setupKeywords() {
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
  keywords["fun"] = FUN;
}
