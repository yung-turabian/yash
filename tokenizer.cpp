#include "include/token.h"

Token* 
createToken(TokenType type, const char * lexeme, int line)
{
		Token* token = nullptr;

		token->type = type;
		token->lexeme = lexeme;
		token->line = line;

		return token;
}
