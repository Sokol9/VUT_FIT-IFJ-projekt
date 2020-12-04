// definice funkce simulujici precedencni syntaktickou analyzu pro zpracovani vyrazu
#include "expr.h"

static int varNumber = 0;

void resetNumber() {
	varNumber = 0;
}

// funkce meni seznam, ktery je ji predan
// probehne-li vse v poradku, seznam obsahuje prave jeden prvek, na ktery funkce vraci ukazatel
// nastane-li v prubehu analyzy chyba, vraci funkce NULL
tokenListItemPtr precedence(tokenListPtr ptr, tSymTablePtr STab, bool resetVarNumber) {
	if(ptr != NULL && STab != NULL) {
		if(!tokenListSemCheck(ptr, STab))
			return NULL;
		if(resetVarNumber)
			resetNumber();
		if(ptr->first != NULL) {
			ptr->active = ptr->first;
			while(ptr->first->next != NULL && ptr->first->next->next != NULL) {
				if(ptr->active == NULL) {
					tokenLastTerm(ptr);
					tokenStartOfExpr(ptr);
					tokenGenerate(ptr, varNumber++);
				} else if(ptr->active->term) {
					if(tokenPrecedence(ptr)) {
						ptr->lastTerm = ptr->active;
						tokenNext(ptr);
					} else {
						tokenLastTerm(ptr);
						tokenStartOfExpr(ptr);
						tokenGenerate(ptr, varNumber++);
					}
				} else
					tokenNext(ptr);
			}
			if(ptr->first->next == NULL)
				return ptr->first;
		}
	}
	return NULL;
}
