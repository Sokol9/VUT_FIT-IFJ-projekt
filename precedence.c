// definice funkce simulujici precedencni syntaktickou analyzu pro zpracovani vyrazu
#include "expr.h"
#include "error.h"

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
tokenListItemPtr precedence(tokenListPtr ptr, tSymTablePtr STab, bool print) {
	if(ptr != NULL && STab != NULL) {
		if(getError() != RESULT_OK || !tokenListSemCheck(ptr, STab))
			return NULL;
	
		if(ptr->first != NULL) {
			ptr->active = ptr->first;
			while(ptr->lastTerm != ptr->active) {
				if(ptr->active == NULL) {
					tokenLastTerm(ptr);
					tokenStartOfExpr(ptr);
					if(!tokenGenerate(ptr, print))
						return NULL;
				} else if(ptr->active->term) {
					if(tokenPrecedence(ptr)) {
						ptr->lastTerm = ptr->active;
						tokenNext(ptr);
					} else {
						tokenLastTerm(ptr);
						tokenStartOfExpr(ptr);
						if(!tokenGenerate(ptr, print))
							return NULL;
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


// funkce pro konvencni vypis retezcovych literalu
void printConvertString(char* input) {
	int i = 0;
	char chr = input[i];
	printf(" string@");
	while (chr != '\0'){
		if ((chr >= 10 && chr <= 32) || chr == 35 || chr == 92)
			printf("\\0%d", chr);
		else if(chr > 0 && chr < 10)
			printf("\\00%d", chr);
		else
        		printf("%c", chr);
		i++;
		chr = input[i];
	}
}
