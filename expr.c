// rozhrani pro zpracovani vyrazu
#include "expr.h"
#include "error.h"
#include "inst.h"
#include <stdio.h>

// velikost pomocnych bufferu pro generovani instrukci
#define BUFFER 15

// navratove hodnoty
#define RET_OK  1
#define RET_ERR 0

static int varNumber = 0;
static char* constPrefixes[5] = {"nil@", "int@", "float@", "string@", "bool@"};

// reset pocitadla pro generovani promennych
void resetNumber() {
	varNumber = 0;
}

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
int tokenAppend(tokenListPtr ptr, tToken* token, tSymTablePtr STab, tokenListItemPtr result) {
	if(ptr != NULL) {
		if(result != NULL) {
			if(ptr->first == NULL) {
				ptr->first  = result;
				ptr->active = result;
				ptr->last   = result;
			} else {
				ptr->last->next = result;
				ptr->last = result;
			}
			return RET_OK;
		} else if(token != NULL) {
			tokenListItemPtr tmp = malloc(sizeof(struct tokenListItem));
			if(tmp != NULL) {
				if(token->type == INT_L)         tmp->type = INT_T;
				else if(token->type == FLOAT_L)  tmp->type = FLOAT64_T;
				else if(token->type == STRING_L) tmp->type = STRING_T;
				else                             tmp->type = UNKNOWN_T;
				tmp->token       = *token;
				tmp->startOfExpr = false;
				tmp->term        = (token->type < ID)? true : false;
				tmp->frameNumber = -1;
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
				if(STab != NULL) {
					if(token->type == ID) {
						tmp->frameNumber = STVarLookUp(STab, token->attr);
						if(tmp->frameNumber != 0)
							if((tmp->type = STVarGetType(STab)) != UNKNOWN_T)
								return RET_OK;
						setError(SEM_DEF_ERROR);
					}
				}
				return RET_OK;
			}
			setError(INTERNAL_ERROR);
		}
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

// semanticke porovnani dvou seznamu a generovani instrukci pro prirazeni hodnot
int tokenListAssign(tokenListPtr dest, tokenListPtr src) {
	if(dest != NULL && src != NULL) {
		dest->active = dest->first;
		src->active  = src->first;
		char pref1[BUFFER] = {'\0'};
		char pref2[BUFFER] = {'\0'};
		while(dest->active != NULL && src->active != NULL) {
			if(dest->active->type != UNKNOWN_T) {
				if(dest->active->type == src->active->type) {
					sprintf(pref1, "LF@f%d$", dest->active->frameNumber);
					sprintf(pref2, "LF@f%d$", src->active->frameNumber);
					ASSIGN(pref1, dest->active->token.attr, src->active->frameNumber, pref2, src->active->token.attr);
				} else {
					setError(SEM_TYPE_ERROR);
					break;
				}
			}
			tokenNext(dest);
			tokenNext(src);	
		}
		if(dest->active == src->active) {
			tokenListDispose(dest);
			tokenListDispose(src);
			return RET_OK;
		}
	}
	tokenListDispose(dest);
	tokenListDispose(src);
	return RET_ERR;
}

// Semanticka kontrola, zda jsou vsechny tokeny stejneho datoveho typu
int tokenListSemCheck(tokenListPtr ptr, tSymTablePtr STab) {
	if(ptr != NULL && STab != NULL) {
		tokenListItemPtr tmp = ptr->first;
		varType type = UNKNOWN_T;

		// nastaveni hodnot pro kontrolu typu a pouzitych operatoru
		while(tmp != NULL) {
			if(tmp->token.type == ID) {
				if(STVarLookUp(STab, tmp->token.attr)) {
					type = STVarGetType(STab);
					if(type == UNKNOWN_T)
						setError(SEM_DEF_ERROR);
					break;
				} else
					break;
			} else if(tmp->token.type > ID) {
				type = tmp->type;
				break;
			} else
				tmp = tmp->next;
		}

		// kontrola
		tmp = ptr->first;
		if(type != UNKNOWN_T) {
			while(tmp != NULL) {
				if(tmp->token.type == ID) {
					int frame = STVarLookUp(STab, tmp->token.attr);
					if(frame) {
						varType t = STVarGetType(STab);
						if(t != type)
							break;
						tmp->frameNumber = frame;
						tmp->type = type;
					} else
 						break;
				} else if(tmp->token.type > ID) {
					if(tmp->type != type)
						break;
					tmp->frameNumber = 0;
				} else if(type == STRING_T)
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

// na zaklade priority mezi lastTerm a active vklada tokenum zacatky podvyrazu
int tokenPrecedence(tokenListPtr ptr) {
	if(ptr->lastTerm == NULL) {
		ptr->first->startOfExpr = true;
		return RET_OK;
	}
	if(ptr->active == NULL)
		return RET_ERR;
	tokenType input_op = ptr->active->token.type;
	tokenType stack_op = ptr->lastTerm->token.type;
	switch(stack_op) {
		case ADD:
		case SUB:
			if(input_op == MULT || input_op == DIV || input_op == OBR)
				break;
			return RET_ERR;
		case MULT:
		case DIV:
			if(input_op == OBR)
				break;
			return RET_ERR;
		case LT:
		case LTEQ:
		case GT:
		case GTEQ:
		case EQ:
		case NEQ:
			if(input_op == CBR || (input_op >= LT && input_op <= NEQ))
				return RET_ERR;
			break;
		case OBR:
			if(input_op == CBR)
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
int tokenGenerate(tokenListPtr ptr) {
	if(ptr != NULL && ptr->startOfExpr != NULL && ptr->startOfExpr->next != NULL) {
		tokenListItemPtr start  = ptr->startOfExpr;
		tokenListItemPtr middle = start->next;
		tokenListItemPtr end    = middle->next;
		if(start == ptr->first)
			ptr->first = middle;
		if(end == ptr->last)
			ptr->last = middle;
		if(end == ptr->active)
			tokenNext(ptr);
		if(middle->token.type < ID) {
			char **endptr = NULL;
			char pref1[BUFFER] = {'\0'};
			char pref2[BUFFER] = {'\0'};
			char cond1[BUFFER] = {'\0'};
			char cond2[BUFFER] = {'\0'};
			middle->frameNumber = -1;
			sprintf(pref1, "LF@f%d$", start->frameNumber);
			sprintf(pref2, "LF@f%d$", end->frameNumber);
			switch(middle->token.type) {
				case ADD:
					VAR_ARITH();
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					if(end->type == STRING_T)
						INST_PREC("CONCAT", middle->token.attr, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					else
						INST_PREC("ADD", middle->token.attr, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					break;

				case SUB:
					VAR_ARITH();
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					INST_PREC("SUB", middle->token.attr, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					break;

				case MULT:
					VAR_ARITH();
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					INST_PREC("MUL", middle->token.attr, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					break;

				case DIV:
					if(end->token.type != ID && strtod(end->token.attr, endptr) == 0.0) {
						setError(SEM_ZERO_ERROR);
						return RET_ERR;
					}
					VAR_ARITH();
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					if(end->type == FLOAT64_T)
						INST_PREC("DIV", middle->token.attr, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					else
						INST_PREC("IDIV", middle->token.attr, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					break;

				case LT:
					VAR_BOOL();
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					INST_PREC("LT", middle->token.attr, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					break;

				case LTEQ:
					VAR_BOOL();
					sprintf(cond1, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(cond1);
					INST_PREC("LT", cond1, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					sprintf(cond2, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(cond2);
					INST_PREC("EQ", cond2, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					INST_PREC("OR", middle->token.attr, -1, cond1, -1, cond2);
					break;

				case GT:
					VAR_BOOL();
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					INST_PREC("GT", middle->token.attr, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					break;

				case GTEQ:
					VAR_BOOL();
					sprintf(cond1, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(cond1);
					INST_PREC("GT", cond1, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					sprintf(cond2, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(cond2);
					INST_PREC("EQ", cond2, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					INST_PREC("OR", middle->token.attr, -1, cond1, -1, cond2);
					break;

				case EQ:
					VAR_BOOL();
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					INST_PREC("EQ", middle->token.attr, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					break;

				case NEQ:
					VAR_BOOL();
					sprintf(cond1, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(cond1);
					INST_PREC("EQ", cond1, start->frameNumber, start->token.attr, end->frameNumber, end->token.attr);
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					DEFVAR_PREC(middle->token.attr);
					INST_PREC("NOT", middle->token.attr, -1, cond1, -1, "");
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

varType tokenListGetFirstType(tokenListPtr tokenList){
	return tokenList->first->type;
}
