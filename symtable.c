// externi definice funkci pro praci s tabulkou symbolu
#include "project.h"
#include "symtable.h"
#include "symtable_private.h"
#include "error.h"

// inicializuje seznam navratovych hodnot
void retListInit(tRetListPtr list) {
	if(list != NULL) {
		list->first  = NULL;
		list->active = NULL;
		list->last   = NULL;
	}
}

// vlozi navratovou hodnotu na konec seznamu
int retListInsert(tRetListPtr list, varType type) {
	if(list != NULL) {
		tRetPtr tmp = malloc(sizeof(struct funcRet));
		if(tmp != NULL) {
			tmp->type = type;
			tmp->next = NULL;
			if(list->first == NULL) {
				list->first  = tmp;
				list->active = tmp;
				list->last   = tmp;
			} else {
				list->last->next = tmp;
				list->last = tmp;
			}
			return 1;
		}
		setError(INTERNAL_ERROR);
	}
	return 0;
}

// posune aktivitu na dalsi prvek
void retListNext(tRetListPtr list) {
	if(list != NULL && list->active != NULL)
		list->active = list->active->next;
}

// zrusi seznam navratovych hodnot
void retListDispose(tRetListPtr list) {
	if(list != NULL) {
		while(list->first != NULL) {
			list->active = list->first;
			list->first = list->first->next;
			free(list->active);
		}
		list->active = NULL;
		list->last   = NULL;
	}
}

// inicializace
int STInit(tSymTablePtr ptr) {
	if(ptr != NULL) {
		ptr->rootPtr = NULL;
		GTInit(&(ptr->rootPtr));
		if(getError() == 0){
			// default aktivni funkce je main
			ptr->activeFunc  = GTInsert(&(ptr->rootPtr), "main", false);
			ptr->activeParam = NULL;
			ptr->activeRet   = NULL;
			ptr->topFrame    = NULL;
			ptr->activeVar   = NULL;
			return 1;
		}
		GTDispose(&(ptr->rootPtr));
	}
	return 0;
}

// vyhledani funkce
/*
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
*/

// zjisteni definovani aktivni funkce
bool STFuncIsDefined(tSymTablePtr ptr) {
	if(ptr != NULL)
		return GTIsDefined(ptr->activeFunc);
	return false;
}

// nastaveni aktivity na funkci
int STFuncSetActive(tSymTablePtr ptr, tGRPtr funcPtr) {
	if(ptr != NULL && funcPtr != NULL) {
		ptr->activeFunc  = funcPtr;
		ptr->activeParam = funcPtr->params;
		ptr->activeRet   = funcPtr->returns;
		return 1;
	}
	return 0;
}

// pridani nove funkce do tabulky symbolu
int STFuncInsert(tSymTablePtr ptr, char *key, bool define) {
	if(ptr != NULL) {
		tGRPtr tmp = GTInsert(&(ptr->rootPtr), key, define);
		if(tmp != NULL) {
			ptr->activeFunc  = tmp;
			ptr->activeParam = tmp->params;
			ptr->activeRet   = tmp->returns;
			return 1;
		}
	}
	return 0;	
}

// nejdrive zjisti, zda byla aktivni funkce jiz drive pouzita nebo definovana
//    pokud ano, zkontroluje datovy typ aktivniho parametru a aktivitu posune na dalsi parametr (NULL, kdyz byl kontrolovan posledni)
//    pokud ne, prida aktivnimu parametru datovy typ, pokud parametr neexistuje, vytvori novy a nastavi ho jako aktivni
int STFuncInsertParamType(tSymTablePtr ptr, varType type) {
	if(ptr != NULL && ptr->activeFunc != NULL) {
		if(ptr->activeFunc->used) {
			if(ptr->activeFunc->randomPCount)
				return 1;
			if(ptr->activeParam != NULL) {
				if(ptr->activeParam->type == type) {
					ptr->activeParam = ptr->activeParam->next;
					return 1;
				}
				setError(SEM_FUNC_ERROR);
			} else
				setError(SEM_FUNC_ERROR);
		} else {
			if(ptr->activeParam == NULL) {
				if((ptr->activeFunc->params = GTAddParamType(NULL, type)) != NULL) {
					ptr->activeParam = ptr->activeFunc->params;
					return 1;
				}
			} else {
				if(ptr->activeParam->id != NULL)
					GTAddParamType(ptr->activeParam, type);
				else {
					if((ptr->activeParam->next = GTAddParamType(NULL, type)) != NULL) {
						ptr->activeParam = ptr->activeParam->next;
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

// prida aktivnimu parametru id, pokud parametr neexistuje, vytvori novy a nastavi ho jako aktivni
int STFuncInsertParamId(tSymTablePtr ptr, char *id) {
	if(ptr != NULL && ptr->activeFunc != NULL) {
		if(ptr->activeFunc->used) {
			if(ptr->activeParam != NULL) {
				if(GTAddParamId(ptr->activeParam, id) != NULL)
					return 1;
			}
			else
				setError(SEM_FUNC_ERROR);
		} else {
			if(ptr->activeParam == NULL) {
				if((ptr->activeFunc->params = GTAddParamId(NULL, id)) != NULL) {
					ptr->activeParam = ptr->activeFunc->params;
					return 1;
				}
			} else {
				if((ptr->activeParam->next = GTAddParamId(NULL, id)) != NULL) {
					ptr->activeParam = ptr->activeParam->next;
					return 1;
				}
			}
		}
	}
	return 0;
}
// pri kontrole parametru kontroluje jejich spravny pocet
int STFuncParamEnd(tSymTablePtr ptr) {
	if(ptr != NULL && ptr->activeFunc != NULL) {
		if(ptr->activeFunc->used) {
			if(ptr->activeParam == NULL)
				return 1;
			setError(SEM_FUNC_ERROR);
		} else
			return 1;
	}
	return 0;
}

// nejdrive zjisti, zda byly funkci jiz definovany navratove hodnoty
//    pokud ano, provadi kontrolu porovnanim dvou seznamu navratovych hodnot
//    pokud ne, vklada aktivni funkci seznam navratovych hodnot
//    je-li volana s list=NULL, pouze nastavuje, ze funkce nema zadne navratove hodnoty
//
//    seznam navratovych hodnot, ktery bude predan funkci se stava prazdnym
int STFuncInsertRet(tSymTablePtr ptr, tRetListPtr list) {
	if(ptr != NULL && list != NULL && ptr->activeFunc != NULL){
		if(ptr->activeFunc->retDefined) {
			while(ptr->activeRet != NULL) {
				if(list->active != NULL) {
					if(ptr->activeRet->type == UNKNOWN_T)
						ptr->activeRet->type = list->active->type;
					else if(list->active->type == UNKNOWN_T);
					else if(ptr->activeRet->type != list->active->type) {
						setError(SEM_FUNC_ERROR);
						retListDispose(list);
						return 0;
					}
					ptr->activeRet = ptr->activeRet->next;
					retListNext(list);
				} else {
					setError(SEM_FUNC_ERROR);
					retListDispose(list);
					return 0;
				}
			}
			if(list->active != NULL)
				setError(SEM_FUNC_ERROR);
			else {
				retListDispose(list);
				return 1;
			}
		} else {
			ptr->activeFunc->returns = list->first;
			list->first = list->active = list->last = NULL;
			ptr->activeFunc->retDefined = true;
			return 1;
		}
	}
	if(list == NULL) {
		ptr->activeFunc->retDefined = true;
		return 1;
	}
	retListDispose(list);
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
	return 0;
}

// ziskani cisla ramce na vrcholu zasobniku
int STGetFrameNumber(tSymTablePtr ptr) {
	if(ptr != NULL)
		return LTGetFrameNumber(ptr->topFrame);
	return 0;
}

// vyhledani promenne
int STVarLookUp(tSymTablePtr ptr, char *key) {
	if(ptr != NULL) {
		int frame;
		tLRPtr tmp = LTSearch(ptr->topFrame, key, &frame);
		if(tmp != NULL) {
			ptr->activeVar = tmp;
			return frame;
		}
		setError(SEM_DEF_ERROR);
	}
	return 0;
}

// vlozeni promenne do tabulky symbolu
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

// vraci identifikator aktivni promenne
char* STVarGetName(tSymTablePtr ptr) {
	if(ptr != NULL)
		return LTGetName(ptr->activeVar);
	return NULL;
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
int STDispose(tSymTablePtr ptr) {
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

