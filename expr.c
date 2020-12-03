// rozhrani pro zpracovani vyrazu
#include "expr.h"
#include "error.h"

// navratove hodnoty
#define RET_OK  1
#define RET_ERR 0

// inicializace seznamu tokenu
void tokenListInit(tokenListPtr ptr) {
	if(ptr != NULL) {
		ptr->first       = NULL;
		ptr->active      = NULL;
		ptr->last        = NULL;
		ptr->lastTerm    = NULL;
		ptr->startOfExpr = NULL;
	}
}

// pridani tokenu na konec seznamu
int tokenAppend(tokenListPtr ptr, tToken token) {
	if(ptr != NULL) {
		tokenListItemPtr tmp = malloc(sizeof(struct tokenListItem));
		if(tmp != NULL) {
			tmp->token       = token;
			tmp->startOfExpr = false;
			tmp->term        = (token.type < ID)? true : false;
			tmp->next        = NULL;
			tmp->prev        = ptr->last;
			if(ptr->first == NULL) {
				ptr->first  = tmp;
				ptr->active = tmp;
				ptr->last   = tmp;
			} else {
				ptr->last->next = tmp;
				ptr->last = tmp;
			}
			return RET_OK;
		}
		setError(INTERNAL_ERROR);
	}
	return RET_ERR;
}

// zruseni seznamu
void tokenListDispose(tokenListPtr ptr) {
	if(ptr != NULL) {
		tokenListItemPtr tmp;
		while(ptr->first != NULL) {
			tmp = ptr->first;
			ptr->first = ptr->first->next;
			free(tmp);
		}
		ptr->active      = NULL;
		ptr->last        = NULL;
		ptr->lastTerm    = NULL;
		ptr->startOfExpr = NULL;
	}
}

// Semanticka kontrola, zda jsou vsechny tokeny stejneho datoveho typu
int tokenListSemCheck(tokenListPtr ptr, tSymTablePtr STab) {
	if(ptr != NULL && STab != NULL) {
		tokenListItemPtr tmp = ptr->first;
		tokenType type = UNKNOWN;

		// nastaveni hodnot pro kontrolu typu a pouzitych operatoru
		while(tmp != NULL) {
			if(tmp->token.type == ID) {
				if(STVarLookUp(STab, tmp->token.attr)) {
					varType t = STVarGetType(STab);
					if(t == INT_T) type = INT_L;
					else if(t == FLOAT64_T) type = FLOAT_L;
					else if(t == STRING_T) type = STRING_L;
					else setError(SEM_DEF_ERROR);
					break;
				} else
					break;
			} else if(tmp->token.type > ID) {
				type = tmp->token.type;
				break;
			} else
				tmp = tmp->next;
		}

		// kontrola
		if(type != UNKNOWN) {
			while(tmp != NULL) {
				if(tmp->token.type == ID) {
					if(STVarLookUp(STab, tmp->token.attr)) {
						varType t = STVarGetType(STab);
						if(t == INT_T && type != INT_L) break;
						else if(t == FLOAT64_T && type != FLOAT_L) break;
						else if(t == STRING_T && type != STRING_L) break;
						else setError(SEM_DEF_ERROR);
						break;
					} else
 						break;
				} else if(tmp->token.type > ID && tmp->token.type != type)
					break;
				else if(type == STRING_L)
					if(tmp->token.type != ADD)
						break;
				tmp = tmp->next;
			}
			if(tmp == NULL)
				return RET_OK;
		}
		setError(SEM_TYPE_ERROR);
	}
	return RET_ERR;
}

// posun aktivity na dalsi token
void tokenNext(tokenListPtr ptr) {
	if(ptr != NULL && ptr->active != NULL)
		ptr->active = ptr->active->next;
}

// na zaklade priority mezi lastTerm a active
//  vklada zacatek podvyrazu za lastTerm, pokud lastTerm=NULL, potom vklada zacatek podvyrazu na prvni prvek seznamu
//  vklada konec podvyrazu pred active, pokud active=NULL, potom vklada konec podvyrazu na posledni prvek seznamu
//    nastavuje startOfExpr
//    vraci nasledujici navratove hodnoty:
//     -1 ... mel-li lastTerm nizsi prioritu nez active
//      0 ... byla-li priorita stejna
//      1 ... mel-li lastTerm vyssi prioritu nez active
int tokenPrecedence(tokenListPtr ptr);

// zpetne hleda posledni term
void tokenLastTerm(tokenListPtr ptr) {
	if(ptr != NULL) {
		while(ptr->lastTerm != NULL) {
			ptr->lastTerm = ptr->lastTerm->prev;
			if(ptr->lastTerm->term)
				break;
		}
	}
}

// od konce seznamu zpetne hleda zacatek podvyrazu
void tokenStartOfExpr(tokenListPtr ptr) {
	if(ptr != NULL) {
		ptr->startOfExpr = ptr->last;
		while(ptr->startOfExpr != NULL) {
			if(ptr->startOfExpr->startOfExpr)
				break;
			ptr->startOfExpr = ptr->startOfExpr->prev;
		}
	}
}

// nahrazuje podvyraz tokenem
//    generuje instrukce
void tokenGenerate(tokenListPtr ptr) {
	if(ptr != NULL && ptr->startOfExpr != NULL && ptr->startOfExpr->next != NULL) {
		tokenListItemPtr middle = ptr->startOfExpr->next;
		tokenListItemPtr end    = middle->next;
		if(end == ptr->active)
			tokenNext(ptr);
		if(end->token.type == CBR) {
			if(ptr->startOfExpr == ptr->first)
				ptr->first = middle;
			if(ptr->last == end)
				ptr->last = middle;
			ptr->startOfExpr->prev->next = tmp;
			tmp->prev                    = ptr->startOfExpr->prev;
			free(ptr->startOfExpr);
			ptr->startOfExpr             = NULL;
			tmp->next                    = end->next;
			end->next->prev              = tmp;
			free(end);
			return;
		}
		switch(ptr->startOfExpr)
	}
}
