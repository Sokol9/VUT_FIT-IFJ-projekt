#include "project.h"

int main() {

	tToken token;
	int error;
	int number = 1;
	eolFlag ef = OPT;
	char *type[7] = {"OP", "KW", "ID", "INT_L", "FLOAT_L", "STRING_L", "TOKEN_EOF"};
	do {
		error = getToken(&token, &ef);
		printf("Token [%03d]: [%d] [%-10s] [%s]\n", number, error, type[token.type], token.attr);
		number++;

	} while(token.type != TOKEN_EOF);
	return 0;
}
