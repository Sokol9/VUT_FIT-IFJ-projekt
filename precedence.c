// definice funkce simulujici precedencni syntaktickou analyzu pro zpracovani vyrazu
#include "expr.h"

#define DEBUG \
	list = ptr->first;\
	while(list != NULL) {\
		printf("%d ", list->token.type);\
		list = list->next;\
	} \
	printf("\n\n");


// funkce meni seznam, ktery je ji predan
// probehne-li vse v poradku, seznam obsahuje prave jeden prvek, na ktery funkce vraci ukazatel
// nastane-li v prubehu analyzy chyba, vraci funkce NULL
// je nutne dealokovat misto pridelene seznamu po volani funkce
tokenListItemPtr precedence(tokenListPtr ptr, tSymTablePtr STab, bool resetVarNumber) {
	if(ptr != NULL && STab != NULL) {

		tokenListItemPtr list;

		if(!tokenListSemCheck(ptr, STab)) {
	printf("\nChybna semantika\n\n");
			return NULL;
		}
	printf("\nSemantika je v poradku\n\n");

		if(resetVarNumber)
			resetNumber();
		if(ptr->first != NULL) {
			ptr->active = ptr->first;
			while(ptr->lastTerm != ptr->active) {
				if(ptr->active == NULL) {
					tokenLastTerm(ptr);
					tokenStartOfExpr(ptr);
	DEBUG
					if(!tokenGenerate(ptr))
						return NULL;
	DEBUG
				} else if(ptr->active->term) {
					if(tokenPrecedence(ptr)) {
						ptr->lastTerm = ptr->active;
						tokenNext(ptr);
					} else {
						tokenLastTerm(ptr);
						tokenStartOfExpr(ptr);
	DEBUG
						if(!tokenGenerate(ptr))
							return NULL;
	DEBUG
					}
				} else
					tokenNext(ptr);
			}
			if(ptr->lastTerm == NULL)
				return ptr->first;
		}
	}
	return NULL;
}
