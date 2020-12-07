// externi definice privatnich funkci pro praci s tabulkou symbolu
#include "project.h"
#include "symtable_private.h"
#include "error.h"

static int frame = 1;

// GLOBALNI TABULKA SYMBOLU
//================================================================

// pomocna funkce pro inicializaci vestavenych funkci jazyka
void IFJ20Func(tGRPtr ptr) {
	if(ptr != NULL) {
		ptr->used       = true;
		ptr->retDefined = true;
	}
}

// inicializace globalni tabulky symbolu
void GTInit(tGRPtr *rootPtr) {

	if(rootPtr != NULL) {
		tGRPtr tmp;

		// func len(s string) (int)
		tmp = GTInsert(rootPtr, "len", true);
		GTAddParam(tmp, STRING_T, "s");
		GTAddRet(tmp, INT_T);
		IFJ20Func(tmp);

		// func inputi() (int, int)
		tmp = GTInsert(rootPtr, "inputi", true);
		GTAddRet(tmp, INT_T);
		GTAddRet(tmp, INT_T);
		IFJ20Func(tmp);

		// func float2int(f float64) (int)
		tmp = GTInsert(rootPtr, "float2int", true);
		GTAddParam(tmp, FLOAT64_T, "f");
		GTAddRet(tmp, INT_T);
		IFJ20Func(tmp);

		// func chr(i int) (string, int)
		tmp = GTInsert(rootPtr, "chr", true);
		GTAddParam(tmp, INT_T, "i");
		GTAddRet(tmp, STRING_T);
		GTAddRet(tmp, INT_T);
		IFJ20Func(tmp);

		// func inputf() (float64, int)
		tmp = GTInsert(rootPtr, "inputf", true);
		GTAddRet(tmp, FLOAT64_T);
		GTAddRet(tmp, INT_T);
		IFJ20Func(tmp);

		// func inputs() (string, int)
		tmp = GTInsert(rootPtr, "inputs", true);
		GTAddRet(tmp, STRING_T);
		GTAddRet(tmp, INT_T);
		IFJ20Func(tmp);

		// func int2float(i int) (float64)
		tmp = GTInsert(rootPtr, "int2float", true);
		GTAddParam(tmp, INT_T, "i");
		GTAddRet(tmp, FLOAT64_T);
		IFJ20Func(tmp);

		// func print(term_1, term_2, ..., term_n)
		tmp = GTInsert(rootPtr, "print", true);
		tmp->randomPCount = true;
		IFJ20Func(tmp);

		// func main() ()
		tmp = GTInsert(rootPtr, "main", false);
		IFJ20Func(tmp);

		// func ord(s string, i int) (int, int)
		tmp = GTInsert(rootPtr, "ord", true);
		GTAddParam(tmp, STRING_T, "s");
		GTAddParam(tmp, INT_T, "i");
		GTAddRet(tmp, INT_T);
		GTAddRet(tmp, INT_T);
		IFJ20Func(tmp);

		// func substr(s string, i int, n int) (string, int)
		tmp = GTInsert(rootPtr, "substr", true);
		GTAddParam(tmp, STRING_T, "s");
		GTAddParam(tmp, INT_T, "i");
		GTAddParam(tmp, INT_T, "n");
		GTAddRet(tmp, STRING_T);
		GTAddRet(tmp, INT_T);
		IFJ20Func(tmp);
	}
}

// vyhledani zaznamu v globalni tabulce
/*
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
*/

// zjistuje, zda je funkce jiz definovana nebo ne
bool GTIsDefined(tGRPtr ptr) {
	return (ptr != NULL)? ptr->defined : false;
}

// vraci jmeno funkce
char* GTGetName(tGRPtr ptr) {
	return (ptr != NULL)? ptr->id : NULL;
}

// pridani zaznamu do globalni tabulky
tGRPtr GTInsert(tGRPtr *rootPtr, char *key, bool define) {
	if(rootPtr != NULL) {
		tGRPtr ptr = *rootPtr;
		if(ptr != NULL) {
			int rel = strcmp(key, ptr->id);
			if(rel == 0) {
				ptr->used = true;
				if(!ptr->defined) {
					ptr->defined = define;
					return ptr;
				}
				if(define) {
					setError(SEM_DEF_ERROR);
					return NULL;
				}
				return ptr;
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
				ptr->params       = NULL;
				ptr->returns      = NULL;
				ptr->defined      = define;
				ptr->used         = false;
				ptr->retDefined   = false;
				ptr->errorFlag    = false;
				ptr->LPtr         = NULL;
				ptr->RPtr         = NULL;
				strcpy(ptr->id, key);
				*rootPtr = ptr;
				return ptr;
			}
			setError(INTERNAL_ERROR);
		}
	}
	return NULL;
}

// prida datovy typ parametru, pokud parametr ptr=NULL, vytvori novy parametr
tParamPtr GTAddParamType(tParamPtr ptr, varType type) {
	if(ptr != NULL)
		ptr->type = type;
	else {
		tParamPtr tmp = malloc(sizeof(struct funcParam));
		if(tmp != NULL) {
			tmp->id   = NULL;
			tmp->type = type;
			tmp->next = NULL;
			return tmp;
		}
		setError(INTERNAL_ERROR);
	}
	return ptr;
}

// prida id parametru, pokud parametr ptr=NULL, vytvori novy parametr
tParamPtr GTAddParamId(tParamPtr ptr, char *id) {
	if(ptr != NULL) {
		ptr->id = malloc(strlen(id)+1);
		if(ptr->id != NULL) {
			strcpy(ptr->id, id);
			return ptr;
		}
		setError(INTERNAL_ERROR);
		return NULL;
	} else {
		tParamPtr tmp = malloc(sizeof(struct funcParam));
                if(tmp != NULL) {
                        tmp->id = malloc(strlen(id)+1);
			if(tmp->id != NULL) {
				strcpy(tmp->id, id);
	                        tmp->type = UNKNOWN_T;
        	                tmp->next = NULL;
                	        return tmp;
			} else
				free(tmp);
                }
                setError(INTERNAL_ERROR);
		return NULL;
	}
}

// prida novy parametr do seznamu parametru funkce
int GTAddParam(tGRPtr ptr, varType type, char *id) {
	if(ptr != NULL) {
		tParamPtr last = ptr->params;
		if(last == NULL) {
			ptr->params = GTAddParamType(NULL, type);
			if(ptr->params != NULL) {
				if(GTAddParamId(ptr->params, id) == NULL) {
					free(ptr->params);
					ptr->params = NULL;
					return 0;
				}
				return 1;
			}
			return 0;
		} else {
			while(last->next != NULL)
				last = last->next;
			last->next = GTAddParamType(NULL, type);
			if(last->next != NULL) {
				if(GTAddParamId(last->next, id) == NULL) {
					free(last->next);
					last->next = NULL;
					return 0;
				}
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

// pomocna funkce pro GTInit
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
				free(par->id);
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
		pointer->frameNumber = frame++;
		pointer->returnFlag = false;
		pointer->rootPtr = NULL;
		if(func != NULL) {
			tParamPtr par = func->params;
			tLRPtr local;
			while(par != NULL) {
				local = LTInsert(pointer, par->id);
				LTSetType(local, par->type);
				par = par->next;
			}
		}
	} else
		setError(INTERNAL_ERROR);
	return pointer;
}

// ziskani cisla ramce
int LTGetFrameNumber(tLFPtr frame) {
	if(frame != NULL)
		return frame->frameNumber;
	return 0;
}

// nastaveni returnFlagu
void LTSetReturnFlag(tLFPtr frame) {
	if(frame != NULL)
		frame->returnFlag = true;
}

// ziskani returnFlagu
bool LTGetReturnFlag(tLFPtr frame) {
	if(frame != NULL)
		return frame->returnFlag;
	return false;
}

// zjistuje, zda je ramec ramcem funkce
bool LTIsFuncFrame(tLFPtr frame) {
	if(frame != NULL)
		return (frame->upper == NULL);
	return false;
}

// vyhledani zaznamu ve strome
tLRPtr LTLookUp (tLRPtr rootPtr, char *key) {
	if(rootPtr == NULL)
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
tLRPtr LTSearch (tLFPtr framePtr, char *key, int *frameNumber) {
	if(framePtr != NULL) {
		tLRPtr ptr = LTLookUp(framePtr->rootPtr, key);
		if(ptr != NULL) {
			*frameNumber = framePtr->frameNumber;
			return ptr;
		} else
			return LTSearch(framePtr->upper, key, frameNumber);
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
			ptr = malloc(sizeof(struct localRec) + strlen(key) + 1);
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

// vrati typ promenne, na jejiz zaznam ukazuje ptr
varType LTGetType(tLRPtr ptr) {
	if(ptr != NULL)
		return ptr->type;
	return UNKNOWN_T;
}

// vraci ukazatel na identifikator promenne
char* LTGetName(tLRPtr ptr) {
	if(ptr != NULL)
		return ptr->id;
	return NULL;
}

// zmeni typ promenne, na jejiz zaznam ukazuje ptr
int LTSetType(tLRPtr ptr, varType type) {
	if(ptr == NULL)
		return 0;
	if(ptr->type == UNKNOWN_T) {
		ptr->type = type;
		return 1;
	}
	if(ptr->type == type)
		return 1;
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
