#include "include/hrry_stdinc.h"
#include "include/token.h"

#include <list>
using namespace std;

char consume();

void scanToken(list<Token> tokens, int line, int col);

list<Token> 
scanTokens()
{
		list<Token> tokens;
		int start, current = 0;
		int line, col = 1;

		while(current >= 1)//len of file)
		{
				start = current;
				scanToken(tokens, line, col);
		}
		
		tokens.push_back(Token{HRRY_EOF, "", line});
		return tokens;
}



void
scanToken(list<Token> tokens, int line, int col)
{
		char ch = consume();
		TokenType type;

		switch(ch) 
		{
				case '(': type = LEFT_PAREN; break;

				case '=':
						addToken(match('=') ? EQUAL_EQUAL : EQUAL);
						break;

				case '--':
						if (match('-')) {
								while(fpeek() != '\n' && !atEnd()) consume();
						} else {
								addToken(MINUS);
						}
						break;

				case ' ':
				case '\r':
				case '\t':
						// ignore whitespace
						return;
				case '\n':
						line++;
						return;

				case '"': string(); break;

				default: 
						if (isDigit(ch)) number; break;

						else 
									fprintf(stderr, "[line %d] Error (filename) : %s", 
												 line, "Unexpected character."); return; //ensure you dont execute,
																																 //continue the scan tho
		}
		
		addToken(tokens, type, line, col);
}

// Eat the next char in source file
char
consume()
{
		return; // source current char
}

// for sake of identifying double tokens, '==' or '||'
bool
match(char expected)
{
		// if at end return false
		
		if(source.currentChar != expected) return false;

		current++;
		return true;
}

void
string()
{
		while(fpeek() != '"' && !atEnd())
		{
				if(fpeek() == '\n') line++;
				consume()
		}

		if(atEnd()) {
				fprintf(stderr, unterminated string);
				return;
		}

		consume(); // the closing "

		// trim " "
		const char* str = source.substring(start + 1, current - 1);
		addToken(STRING, str);
}

bool
isDigit(char c)
{
		return c >= '0' && c <= '9';
}

void
number()
{
		while(isDigit(fpeek())) consume();

		if(fpeek() == '.' && isDigiet(fpeekNext()) { // ensure next char isnt EOF return '/0'
																								 // else return char
				consume(); //consume the '.'
				while(isDigit(peek())) consume();
		}

		addToken(HRRY_NUMBER,
				Double.parseDouble(source.substring(start, current)));

}

void
addToken(list<Token> tokens, TokenType type, int line, int col)
{
		addToken(tokens, type, line, col);
}

void
addToken(list<Token> tokens, TokenType type, int line, int col)
{
		char* text = string(start, current);

		tokens.push_back(Token{type, text, line});
}
