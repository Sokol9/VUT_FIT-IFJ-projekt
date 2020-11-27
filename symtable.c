// externi definice funkci pro praci s tabulkou symbolu
#include "project.h"
#include "symtable.h"
#include "error.h"

// inicializace
int STInit(tSymTablePtr ptr) {
	if(ptr != NULL) {
		if(GTInit(&(ptr->rootPtr))) {
			// default aktivni funkce je main
			ptr->activeFunc = GTLookUp(ptr->rootPtr, "main");
			ptr->topFrame = NULL;
			ptr->activeVar = NULL;
			return 1;
		}
			GTDispose(&(ptr->rootPtr));
	}
	return 0;
}

// vyhledani funkce
int STFuncLookUp(tSymTablePtr ptr, char *key) {
	if(ptr != NULL) {
		tGRPtr tmp = GTLookUp(ptr->rootPtr, key);
		if(tmp != NULL) {
			ptr->activeFunc = tmp;
			return 1;
		}
	}
	return 0;
}

// zjisteni definovani aktivni funkce
bool STFuncIsDefined(tSymTablePtr ptr) {
	if(ptr != NULL)
		return GTIsDefined(ptr->activeFunc);
	return false;
}

// nastaveni aktivity na funkci
int STFuncSetActive(tSymTablePtr ptr, tGRPtr funcPtr) {
	if(ptr != NULL && funcPtr != NULL) {
		ptr->activeFunc = funcPtr;
		return 1;
	}
	return 0;
}

// pridani nove funkce do tabulky symbolu
int STFuncInsert(tSymTablePtr ptr, char *key, bool define) {
	if(ptr != NULL) {
		tGRPtr tmp = GTInsert(ptr->rootPtr, key, define);
		if(tmp != NULL) {
			ptr->activeFunc = tmp;
			return 1;
		}
	}
	return 0;	
}

// prida novy parametr do seznamu parametru aktivni funkce
int STFuncAddParam(tSymTablePtr ptr, varType type, char *id) {
	if(ptr != NULL)
		return GTAddParam(ptr->activeFunc, type, id);
	return 0;
}

// prida novou navratovou hodnotu do seznamu navratovych hodnot aktivni funkce
int STFuncAddRet(tSymTablePtr ptr, varType type) {
	if(ptr != NULL)
		return GTAddRet(ptr->activeFunc, type);
	return 0;
}

// vytvoreni noveho lokalniho ramce
int STCreateFrame(tSymTablePtr ptr, bool func) {
	if(ptr != NULL) {
		tLFPtr tmp = LTCreateFrame(ptr->topFrame, (func)? ptr->activeFunc : NULL);
		if(tmp != NULL) {
			ptr->topFrame = tmp;
			return 1;
		}
	}
	return 0
}

// vyhledani promenne
int STVarLookUp(tSymTablePtr ptr, char *key) {
	if(ptr != NULL) {
		tLRPtr tmp = LTSearch(ptr->topFrame, key);
		if(tmp != NULL) {
			ptr->activeVar = tmp;
			return 1;
		}
	}
	return 0;
}

// // vlozeni promenne do tabulky symbolu
int STVarInsert(tSymTablePtr ptr, char *key) {
	if(ptr != NULL) {
		tLRPtr tmp = LTInsert(ptr->topFrame, key);
		if(tmp != NULL) {
			ptr->activeVar = tmp;
			return 1;
		}
	}
	return 0;
}

// zjisti datovy typ aktivni promenne
varType STVarGetType(tSymTablePtr ptr) {
	if(ptr != NULL)
		return LTGetType(ptr->activeVar);
	return UNKNOWN_T;
}

// zmena datoveho typu aktivni promenne
int STVarSetType(tSymTablePtr ptr, varType type) {
	if(ptr != NULL)
		return LTSetType(ptr->activeVar, type);
	return 0;
}

// smaze ramec na vrcholu zasobniku
int STDeleteFrame(tSymTablePtr ptr) {
	if(ptr != NULL) {
		ptr->topFrame = LTDeleteFrame(ptr->topFrame);
		return 1;
	}
	return 0;
}

// odstrani tabulku symbolu
int STDispose(tSymTablePtr *ptr) {
	if(ptr != NULL) {
		GTDispose(&(ptr->rootPtr));
		while(ptr->topFrame != NULL)
			ptr->topFrame = LTDeleteFrame(ptr->topFrame);
		ptr->activeFunc = NULL;
		ptr->activeVar = NULL;
		return 1;
	}
	return 0;
}

