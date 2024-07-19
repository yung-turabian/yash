#ifndef HRRY_TOKEN_H
#define HRRY_TOKEN_H

#include "hrry_stdinc.h"

typedef enum TokenType {
		
		// Single character
		LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
		COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

		// one or two character
		BANG, BANG_EQUAL,
		EQUAL, EQUAL_EQUAL,
		GREATER, GREATER_EQUAL,
		LESS, LESS_EQUAL,

		// Literals
		HRRY_IDENTIFIER, HRRY_STRING, HRRY_NUMBER,

		// keywords
		HRRY_AND, HRRY_ELSE, HRRY_FALSE, FUN, HRRY_FOR, HRRY_IF, HRRY_NULL, HRRY_OR,
		HRRY_PRINT, HRRY_RETURN, HRRY_TRUE, VAR, HRRY_WHILE,

		hrry_string, // "x"

		hrry_pipe, // |

		hrry_andand, // &&

		oror, // || 

		hrry_redirect, // > <

		comment, // #

		background, // &
		
		end, // \n or ;

		HRRY_EOF
} TokenType;

typedef struct Token {
		TokenType type;
		const char * lexeme;
		int line;
} Token;

Token* createToken(TokenType type, const char * lexeme, int line);

#endif
