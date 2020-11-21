#include "project.h"

int main() {

	tKWPtr keyWords = KWInit();
	tToken token;
	int error;
	int number = 1;
	eolFlag ef = OPT;
	char *type[] = {"UNKNOWN", "ADD", "SUB", "MULT", "DIV", "SEM", "OBR", "CBR", "OB", "CB", "LT", "LTEQ", "GT", "GTEQ", "EQ", "NEQ", "ASG", "DEF", "COM", "KW_ELSE", "KW_FLOAT64", "KW_FOR", "KW_FUNC", "KW_IF", "KW_INT", "KW_PACKAGE", "KW_RETURN", "KW_STRING", "ID", "US", "INT_L", "FLOAT_L", "STRING_L", "TOKEN_EOF"};
	do {
		error = getToken(&token, &ef, keyWords);
		printf("Token [%03d]: [%d] [%-10s] [%s]\n", number, error, type[token.type], token.attr);
		number++;

	} while(token.type != TOKEN_EOF);
	KWDispose(&keyWords);
	return 0;
}
