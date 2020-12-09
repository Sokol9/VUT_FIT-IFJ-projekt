#include "project.h"
#include "syntax.h"
#include "symtable.h"
#include "error.h"

int main() {

	tKWPtr keyWords = KWInit();
	tToken* token = malloc(sizeof(tToken));
	tokenInit(token);

	bool sucess = true;
	tSymTablePtr STab = malloc (sizeof(struct SymTable));
	if (!STab) setError(INTERNAL_ERROR); else printf("STab allocation success\n\n");
	if (STInit(STab)) printf("STab init success\n\n");

	rule_prog(token, STab, keyWords, &sucess);	
	
	if(!STFuncDefCheck(STab->rootPtr)) setError(SEM_DEF_ERROR);
	STDispose(STab);
	free(STab);
	KWDispose(&keyWords);
	tokenDispose(token);//with free inside
	//printf("eroro: %d\n\n", getError());
	return getError() ;
}
