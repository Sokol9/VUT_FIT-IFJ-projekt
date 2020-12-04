// rozhrani pro zpracovani vyrazu
#include "expr.h"
#include "error.h"

//================
// PROVIZORNE!!!
#include <stdio.h>
//================

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
//  vklada zacatek podvyrazu do prvku za lastTerm, pokud je NULL, vklada zacatek podvyrazu do prvniho prvku
//  nepovede-li si vlozit zacatek noveho podvyrazu, je nutne zpracovat dosavadni podvyraz
//    nastavuje startOfExpr tokenu
//    vraci nasledujici navratove hodnoty:
//      RET_ERR ... nepovedlo se vlozit startOfExpr tokenu .. nasleduje zpracovani podvyrazu
//      RET_OK  ... povedlo se vlozit startOfExpr tokenu   .. nasleduje prochazeni seznamu
int tokenPrecedence(tokenListPtr ptr) {
	if(ptr->lastTerm == NULL) {
		ptr->first->startOfExpr = true;
		return RET_OK;
	}
	if(ptr->active == NULL)
		return RET_ERR;
	tokenType op = ptr->active->token.type;
	switch(ptr->lastTerm->token.type) {
		case ADD:
		case SUB:
			if(op == ADD || op == SUB || op == CBR)
				return RET_ERR;
			break;
		case MULT:
		case DIV:
			if(op == OBR)
				break;
			return RET_ERR;
		case OBR:
			if(op == CBR)
				return RET_ERR;
			break;
		default:
			setError(SYN_ERROR);
			break;
	}
	ptr->lastTerm->next->startOfExpr = true;
	return RET_OK;
}

// zpetne hleda posledni term
void tokenLastTerm(tokenListPtr ptr) {
	if(ptr != NULL && ptr->lastTerm != NULL) {
		do {
			ptr->lastTerm = ptr->lastTerm->prev;
		} while(ptr->lastTerm != NULL && !ptr->lastTerm->term);
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

// nahrazuje podvyraz tokenem, ktery predstavuje nove vygenerovanou pomocnou promennou
int tokenGenerate(tokenListPtr ptr, int varNumber) {
	if(ptr != NULL && ptr->startOfExpr != NULL && ptr->startOfExpr->next != NULL) {
		tokenListItemPtr middle = ptr->startOfExpr->next;
		tokenListItemPtr end    = middle->next;
		if(ptr->startOfExpr == ptr->first)
			ptr->first = middle;
		if(ptr->last == end)
			ptr->last = middle;
		if(end == ptr->active)
			tokenNext(ptr);
		if(middle->token.type < ID) {
			char **endptr = NULL;
			switch(middle->token.type) {
				case ADD:
					middle->token.type = end->token.type;
					sprintf(middle->token.attr, "prec$%d", varNumber);
					if(end->token.type == STRING_L) {
						// generovani instrukce CONCAT
						printf("DEFVAR [%-10s]\n", middle->token.attr);
						printf("CONCAT [%-10s] [%-10s] [%-10s]\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					} else {
						// generovani instrukce ADD
						printf("DEFVAR [%-10s]\n", middle->token.attr);
						printf("ADD [%-10s] [%-10s] [%-10s]\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					}
					break;

				case SUB:
					middle->token.type = end->token.type;
					sprintf(middle->token.attr, "prec$%d", varNumber);
					// generovani instrukce SUB
					printf("DEFVAR [%-10s]\n", middle->token.attr);
					printf("SUB [%-10s] [%-10s] [%-10s]\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					break;

				case MULT:
					middle->token.type = end->token.type;
					sprintf(middle->token.attr, "prec$%d", varNumber);
					// generovani instrukce MUL
					printf("DEFVAR [%-10s]\n", middle->token.attr);
					printf("MUL [%-10s] [%-10s] [%-10s]\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					break;

				case DIV:
					middle->token.type = end->token.type;
					sprintf(middle->token.attr, "prec$%d", varNumber);
					if(strtod(end->token.attr, endptr) == 0.0) {
						setError(SEM_ZERO_ERROR);
						return RET_ERR;
					}
					if(end->token.type == FLOAT_L) {
						// generovani instrukce DIV
						printf("DEFVAR [%-10s]\n", middle->token.attr);
						printf("DIV [%-10s] [%-10s] [%-10s]\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					} else {
						// generovani instrukce IDIV
						printf("DEFVAR [%-10s]\n", middle->token.attr);
						printf("IDIV [%-10s] [%-10s] [%-10s]\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					}
					break;

				default:
					setError(SYN_ERROR);
					return RET_ERR;
			}
		}
		middle->term = false;
		middle->prev = ptr->startOfExpr->prev;
		if(ptr->startOfExpr->prev != NULL)
			ptr->startOfExpr->prev->next = middle;
		free(ptr->startOfExpr);
		ptr->startOfExpr = NULL;
		middle->next = end->next;
		if(end->next != NULL)
			end->next->prev = middle;
		free(end);
		return RET_OK;
	}
	return RET_ERR;
}
