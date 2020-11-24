#include "project.h"
#include "syntax.h"

int main() {

	tKWPtr keyWords = KWInit();
	tToken token;
	int error;
	int number = 1;
	
	bool sucess = true;

	char *type[] = {"UNKNOWN", "ADD", "SUB", "MULT", "DIV", "SEM", "OBR", "CBR", "OB", "CB", "LT", "LTEQ", "GT", "GTEQ", "EQ", "NEQ", "ASG", "DEF", "COM", "KW_ELSE", "KW_FLOAT64", "KW_FOR", "KW_FUNC", "KW_IF", "KW_INT", "KW_PACKAGE", "KW_RETURN", "KW_STRING", "ID", "US", "INT_L", "FLOAT_L", "STRING_L", "TOKEN_EOF"};

	rule_prog(&token, keyWords, &sucess);	

	do {
		error = getToken(&token, keyWords);
		printf("Token [%03d]: [%d] [%-10s] [\\n = %d] [%s]\n", number, error, type[token.type], (token.eolFlag)?1:0, token.attr);
		number++;
	
	} while(token.type != TOKEN_EOF);
	KWDispose(&keyWords);
	return 0;
}
