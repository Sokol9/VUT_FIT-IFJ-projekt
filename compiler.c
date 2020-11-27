#include "project.h"
#include "syntax.h"
#include "symtable.h"
#include "error.h"

int main() {

	tKWPtr keyWords = KWInit();
	tToken token;
	bool sucess = true;
	tSymTablePtr STab = malloc (sizeof(struct SymTable));
	if (!STab) setError(INTERNAL_ERROR); else printf("STab init sucess\n\n");

	rule_prog(&token, STab, keyWords, &sucess);	
	
	STDispose(STab);
	KWDispose(&keyWords);
	return getError() ;
}
