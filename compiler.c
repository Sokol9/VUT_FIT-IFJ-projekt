#include "project.h"
#include "stack.h"
#include "syntax.h"
#include <stdlib.h>
int main() {

	tToken token;
	int error;
	int number = 1;
	eolFlag ef = OPT;
	char *type[7] = {"OP", "KW", "ID", "INT_L", "FLOAT_L", "STRING_L", "TOKEN_EOF"};
	sStack* S = malloc(sizeof(sStack));
	if (S == NULL)
		return 1; //chyba 99
	 

	printf("%d\n",S->top);

	StackInit(S);	
	
		
	printf("%d\n",S->top);

	do {

		error = getToken(&token, &ef);

		printf("Token [%03d]: [%d] [%-10s] [%s]\n", number, error, type[token.type], token.attr);
		number++;
		
		check(StackTop(S), token.type, S);

	} while(token.type != TOKEN_EOF);
	return 0;
}
