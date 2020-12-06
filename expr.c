// rozhrani pro zpracovani vyrazu
#include "expr.h"
#include "error.h"
#include <stdio.h>

// navratove hodnoty
#define RET_OK  1
#define RET_ERR 0

static int varNumber = 0;

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
int tokenAppend(tokenListPtr ptr, tToken* token) {
	if(ptr != NULL) {
		tokenListItemPtr tmp = malloc(sizeof(struct tokenListItem));
		if(tmp != NULL) {
			if(token->type == INT_L) tmp->type = INT_T;
			else if(token->type == FLOAT_L) tmp->type = FLOAT64_T;
			else if(token->type == STRING_L) tmp->type = STRING_T;
			else tmp->type = UNKNOWN_T;
			tmp->token       = *token;
			tmp->startOfExpr = false;
			tmp->term        = (token->type < ID)? true : false;
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
					if(STVarLookUp(STab, tmp->token.attr)) {
						varType t = STVarGetType(STab);
						if(t != type)
							break;
						tmp->type = type;
					} else
 						break;
				} else if(tmp->token.type > ID && tmp->type != type)
					break;
				else if(type == STRING_T)
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
			char cond1[15] = {'\0'};
			char cond2[15] = {'\0'};
			switch(middle->token.type) {
				case ADD:
					middle->token.type = ID;
					middle->type = end->type;
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					if(end->type == STRING_T) {
						// generovani instrukce CONCAT
						printf("DEFVAR %s\n", middle->token.attr);
						printf("CONCAT %s %s %s\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					} else {
						// generovani instrukce ADD
						printf("DEFVAR %s\n", middle->token.attr);
						printf("ADD %s %s %s\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					}
					break;

				case SUB:
					middle->token.type = ID;
					middle->type = end->type;
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					// generovani instrukce SUB
					printf("DEFVAR %s\n", middle->token.attr);
					printf("SUB %s %s %s\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					break;

				case MULT:
					middle->token.type = ID;
					middle->type = end->type;
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					// generovani instrukce MUL
					printf("DEFVAR %s\n", middle->token.attr);
					printf("MUL %s %s %s\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					break;

				case DIV:
					middle->token.type = ID;
					middle->type = end->type;
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					if(end->token.type != ID && strtod(end->token.attr, endptr) == 0.0) {
						setError(SEM_ZERO_ERROR);
						return RET_ERR;
					}
					if(end->type == FLOAT64_T) {
						// generovani instrukce DIV
						printf("DEFVAR %s\n", middle->token.attr);
						printf("DIV %s %s %s\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					} else {
						// generovani instrukce IDIV
						printf("DEFVAR %s\n", middle->token.attr);
						printf("IDIV %s %s %s\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					}
					break;

				case LT:
					middle->token.type = ID;
					middle->type = BOOL_T;
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					// generovani instrukce LT
					printf("DEFVAR %s\n", middle->token.attr);
					printf("LT %s %s %s\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					break;

				case LTEQ:
					middle->token.type = ID;
					middle->type = BOOL_T;
					// generovani instrukce LT
					sprintf(cond1, "LF@prec$%d", varNumber++);
					printf("DEFVAR %s\n", cond1);
					printf("LT %s %s %s\n", cond1, ptr->startOfExpr->token.attr, end->token.attr);
					// generovani instrukce EQ
					sprintf(cond2, "LF@prec$%d", varNumber++);
					printf("DEFVAR %s\n", cond2);
					printf("EQ %s %s %s\n", cond2, ptr->startOfExpr->token.attr, end->token.attr);
					// generovani instrukce OR
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					printf("DEFVAR %s\n", middle->token.attr);
					printf("OR %s %s %s\n", middle->token.attr, cond1, cond2);
					break;

				case GT:
					middle->token.type = ID;
					middle->type = BOOL_T;
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					// generovani instrukce GT
					printf("DEFVAR %s\n", middle->token.attr);
					printf("GT %s %s %s\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					break;

				case GTEQ:
					middle->token.type = ID;
					middle->type = BOOL_T;
					// generovani instrukce GT
					sprintf(cond1, "LF@prec$%d", varNumber++);
					printf("DEFVAR %s\n", cond1);
					printf("GT %s %s %s\n", cond1, ptr->startOfExpr->token.attr, end->token.attr);
					// generovani instrukce EQ
					sprintf(cond2, "LF@prec$%d", varNumber++);
					printf("DEFVAR %s\n", cond2);
					printf("EQ %s %s %s\n", cond2, ptr->startOfExpr->token.attr, end->token.attr);
					// generovani instrukce OR
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					printf("DEFVAR %s\n", middle->token.attr);
					printf("OR %s %s %s\n", middle->token.attr, cond1, cond2);
					break;

				case EQ:
					middle->token.type = ID;
					middle->type = BOOL_T;
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					// generovani instrukce EQ
					printf("DEFVAR %s\n", middle->token.attr);
					printf("EQ %s %s %s\n", middle->token.attr, ptr->startOfExpr->token.attr, end->token.attr);
					break;

				case NEQ:
					middle->token.type = ID;
					middle->type = BOOL_T;
					// generovani instrukce EQ
					sprintf(cond1, "LF@prec$%d", varNumber++);
					printf("DEFVAR %s\n", cond1);
					printf("EQ %s %s %s\n", cond1, ptr->startOfExpr->token.attr, end->token.attr);
					// generovani instrukce NOT
					sprintf(middle->token.attr, "LF@prec$%d", varNumber++);
					printf("DEFVAR %s\n", middle->token.attr);
					printf("NOT %s %s\n", middle->token.attr, cond1);
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
