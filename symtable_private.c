// externi definice privatnich funkci pro praci s tabulkou symbolu
#include "project.h"
#include "symtable_private.h"
#include "error.h"

// GLOBALNI TABULKA SYMBOLU
//================================================================

// inicializace globalni tabulky symbolu
int GTInit(tGRPtr *rootPtr) {

	tGRPtr tmp;
	// func len(s string) (int)
	if ((tmp = GTInsert(rootPtr, "len", true)) == NULL)
		return 0;
	GTAddParam(tmp, STRING_T, "s");
	GTAddRet(tmp, INT_T);

	// func inputi() (int, int)
	if ((tmp = GTInsert(rootPtr, "inputi", true)) == NULL)
		return 0;
	GTAddRet(tmp, INT_T);
	GTAddRet(tmp, INT_T);

	// func float2int(f float64) (int)
	if ((tmp = GTInsert(rootPtr, "float2int", true)) == NULL)
		return 0;
	GTAddParam(tmp, FLOAT64_T, "f");
	GTAddRet(tmp, INT_T);

	// func chr(i int) (string, int)
	if ((tmp = GTInsert(rootPtr, "chr", true)) == NULL)
		return 0;
	GTAddParam(tmp, INT_T, "i");
	GTAddRet(tmp, STRING_T);
	GTAddRet(tmp, INT_T);

	// func inputf() (float64, int)
	if ((tmp = GTInsert(rootPtr, "inputf", true)) == NULL)
		return 0;
	GTAddRet(tmp, FLOAT64_T);
	GTAddRet(tmp, INT_T);

	// func inputs() (string, int)
	if ((tmp = GTInsert(rootPtr, "inputs", true)) == NULL)
		return 0;
	GTAddRet(tmp, STRING_T);
	GTAddRet(tmp, INT_T);

	// func int2float(i int) (float64)
	if ((tmp = GTInsert(rootPtr, "int2float", true)) == NULL)
		return 0;
	GTAddParam(tmp, INT_T, "i");
	GTAddRet(tmp, FLOAT64_T);

	// func print(term_1, term_2, ..., term_n)
	if ((tmp = GTInsert(rootPtr, "print", true)) == NULL)
		return 0;
	tmp->randomPCount = true;

	// func main() ()
	if ((tmp = GTInsert(rootPtr, "main", false)) == NULL)
		return 0;

	// func ord(s string, i int) (int, int)
	if ((tmp = GTInsert(rootPtr, "ord", true)) == NULL)
		return 0;
	GTAddParam(tmp, STRING_T, "s");
	GTAddParam(tmp, INT_T, "i");
	GTAddRet(tmp, INT_T);
	GTAddRet(tmp, INT_T);

	// func substr(s string, i int, n int) (string, int)
	if ((tmp = GTInsert(rootPtr, "substr", true)) == NULL)
		return 0;
	GTAddParam(tmp, STRING_T, "s");
	GTAddParam(tmp, INT_T, "i");
	GTAddParam(tmp, INT_T, "n");
	GTAddRet(tmp, STRING_T);
	GTAddRet(tmp, INT_T);

	return 1;
}

// vyhledani zaznamu v globalni tabulce
tGRPtr GTLookUp(tGRPtr rootPtr, char *key) {
	if(rootPtr == NULL)
		return NULL;
	else {
		int rel = strcmp(key, rootPtr->id);
		if(rel == 0)
			return rootPtr;
		else {
			if(rel < 0)
				return GTLookUp(rootPtr->LPtr, key);
			else
				return GTLookUp(rootPtr->RPtr, key);
		}
	}
}

// zjistuje, zda je funkce jiz definovana nebo ne
bool GTIsDefined(tGRPtr ptr) {
	return (ptr != NULL)? ptr->defined : false;
}

// pridani zaznamu do globalni tabulky
tGRPtr GTInsert(tGRPtr *rootPtr, char *key, bool define) {
	if(rootPtr != NULL) {
		tGRPtr ptr = *rootPtr;
		if(ptr != NULL) {
			int rel = strcmp(key, ptr->id);
			if(rel == 0) {
				if(!ptr->defined) {
					ptr->defined = define;
					return ptr;
				}
				if(define) {
					setError(SEM_DEF_ERROR);
					return NULL;
				}
			} else {
				if(rel < 0)
					return GTInsert(&(ptr->LPtr), key, define);
				else
					return GTInsert(&(ptr->RPtr), key, define);
			}
		} else {
			ptr = malloc(sizeof(struct globalRec) + strlen(key) + 1);
			if(ptr != NULL) {
				ptr->randomPCount = false;
				ptr->params = NULL;
				ptr->returns = NULL;
				ptr->defined = define;
				ptr->LPtr = NULL;
				ptr->RPtr = NULL;
				strcpy(ptr->id, key);
				*rootPtr = ptr;
				return ptr;
			}
			setError(INTERNAL_ERROR);
		}
	}
	return NULL;
}

// prida novy parametr do seznamu parametru funkce
int GTAddParam(tGRPtr ptr, varType type, char *id) {
	if(ptr != NULL) {
		tParamPtr tmp = malloc(sizeof(struct funcParam) + strlen(id) + 1);
		if(tmp != NULL) {
			tmp->type = type;
			tmp->next = NULL;
			strcpy(tmp->id, id);
			tParamPtr last = ptr->params;
			if(last == NULL)
				ptr->params = tmp;
			else {
				while(last->next != NULL)
					last = last->next;
				last->next = tmp;
			}
			return 1;
		} else
			setError(INTERNAL_ERROR);
	}
	return 0;
}

// prida novou navratovou hodnotu do seznamu navratovych hodnot funkce
int GTAddRet(tGRPtr ptr, varType type) {
	if(ptr != NULL) {
		tRetPtr tmp = malloc(sizeof(struct funcRet));
		if(tmp != NULL) {
			tmp->type = type;
			tmp->next = NULL;
			tRetPtr last = ptr->returns;
			if(last == NULL)
				ptr->returns = tmp;
			else {
				while(last->next != NULL)
					last = last->next;
				last->next = tmp;
			}
			return 1;
		} else
			setError(INTERNAL_ERROR);
	}
	return 0;
}

// smazani globalni tabulky
void GTDispose(tGRPtr *rootPtr) {
	if(rootPtr != NULL) {
		tGRPtr ptr = *rootPtr;
		if(ptr != NULL) {
			GTDispose(&(ptr->LPtr));
			GTDispose(&(ptr->RPtr));
			tParamPtr par = ptr->params;
			while(par != NULL) {
				ptr->params = par->next;
				free(par);
				par = ptr->params;
			}
			tRetPtr ret = ptr->returns;
			while(ret != NULL) {
				ptr->returns = ret->next;
				free(ret);
				ret = ptr->returns;
			}
			free(ptr);
			*rootPtr = NULL;
		}
	}
}

// LOKALNI TABULKA SYMBOLU
//================================================================

// vytvoreni a inicializace nove urovne lokalni tabulky symbolu
tLFPtr LTCreateFrame(tLFPtr upper, tGRPtr func) {
	tLFPtr pointer = malloc(sizeof(struct localFrame));
	if(pointer != NULL) {
		pointer->upper = upper;
		pointer->rootPtr = NULL;
		if(func != NULL) {
			tParamPtr par = func->params;
			tLRPtr local;
			while(par != NULL) {
				local = LTInsert(pointer, par->id);
				LTAddType(local, par->type);
				par = par->next;
			}
		}
	} else
		setError(INTERNAL_ERROR);
	return pointer;
}

// vyhledani zaznamu ve strome
tLRPtr LTLookUp (tLRPtr rootPtr, char *key) {
	ef(rootPtr == NULL)
		return NULL;

	int result = strcmp(key, rootPtr->id);
	if(result == 0)
		return rootPtr;

	if(result < 0)
		return LTLookUp(rootPtr->LPtr, key);

	else
		return LTLookUp(rootPtr->RPtr, key);
}

// prohledani ramce/ramcu
tLRPtr LTSearch (tLFPtr framePtr, char *key) {
	if(framePtr != NULL) {
		tLRPtr ptr = LTLookUp(framePtr->rootPtr, key);
		if(ptr != NULL)
			return ptr;
		else
			return LTSearch(framePtr->upper, key);
	}
	return NULL;
}

// vlozeni zaznamu do stromu
tLRPtr LTInsertToTree(tLRPtr *rootPtr, char *key) {
	if(rootPtr != NULL) {
		tLRPtr ptr = *rootPtr;
		if(ptr != NULL) {
			int rel = strcmp(key, ptr->id);
			if(rel == 0) {
				setError(SEM_DEF_ERROR);
				return NULL;
			} else {
				if(rel < 0)
					return LTInsertToTree(&(ptr->LPtr), key);
				else
					return LTInsertToTree(&(ptr->RPtr), key);
			}
		} else {
			ptr = malloc(sizeof(struct localRec));
			if(ptr != NULL) {
				strcpy(ptr->id, key);
				ptr->LPtr = NULL;
				ptr->RPtr = NULL;
				ptr->type = UNKNOWN_T;
				*rootPtr = ptr;
				return ptr;
			}
			setError(INTERNAL_ERROR);
		}
	}
	return NULL;
}

// vlozeni zaznamu do daneho ramce
tLRPtr LTInsert(tLFPtr framePtr, char *key) {
	if(framePtr != NULL) {
		return LTInsertToTree(&(framePtr->rootPtr), key);
	}
	return NULL;
}

// zmeni typ promenne, na jejiz zaznam ukazuje ptr
int LTAddType(tLRPtr ptr, varType type) {
	if(ptr == NULL)
		return 0;
	if(ptr->type == UNKNOWN_T) {
		ptr->type = type;
		return 1;
	}
	setError(SEM_TYPEDEF_ERROR);
	return 0;
}

// smaze BVS
void LTDeleteTree (tLRPtr *rootPtr) {

	if(rootPtr != NULL) {
		tLRPtr ptr = *rootPtr;
		if(ptr != NULL) {
			LTDeleteTree(&(ptr->LPtr));
			LTDeleteTree(&(ptr->RPtr));
			free(ptr);
			*rootPtr = NULL;
		}
	}
}

// zrusi nejvice zanoreny ramec
tLFPtr LTDeleteFrame(tLFPtr framePtr) {
	if(framePtr != NULL) {
		tLFPtr tmp = framePtr;
		framePtr = framePtr->upper;
		LTDeleteTree(&(tmp->rootPtr));
		free(tmp);
	}
	return framePtr;
}
