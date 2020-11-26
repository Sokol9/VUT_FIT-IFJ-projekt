#include "project.h"
#include "syntax.h"
#include "symtable.h"

int main() {

	tKWPtr keyWords = KWInit();
	tToken token;
	bool sucess = true;
	tGRPtr 	GTable = GTInit();

	if (!GTable) setError(INTERNAL_ERROR); else	printf("GT init sucess\n\n");


	//char *type[] = {"UNKNOWN", "ADD", "SUB", "MULT", "DIV", "SEM", "OBR", "CBR", "OB", "CB", "LT", "LTEQ", "GT", "GTEQ", "EQ", "NEQ", "ASG", "DEF", "COM", "KW_ELSE", "KW_FLOAT64", "KW_FOR", "KW_FUNC", "KW_IF", "KW_INT", "KW_PACKAGE", "KW_RETURN", "KW_STRING", "ID", "US", "INT_L", "FLOAT_L", "STRING_L", "TOKEN_EOF"};

	rule_prog(&token, GTable, keyWords, &sucess);	

	
	KWDispose(&keyWords);
	return getError() ;
}
