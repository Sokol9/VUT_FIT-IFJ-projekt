// externi definice funkci pro praci s tabulkou klicovych slov

#include "project.h"

// provede rekurzivni vlozeni klicoveho slova do tabulky klicovych slov
// pomocna funkce pro KWInit
//    plati zde, ze pokud ma polozka mensi 'key', ma i mensi 'value'
int KWInsert(tKWPtr *rootPtr, char *key, tokenType value) {

	tKWPtr ptr = *rootPtr;
	if(ptr != NULL) {
		// vzhledem k tomu, ze jsou klicova slova ruzna, nikdy mi nenastane situace, ze by se hodnoty shodovaly
		// take nikdy nebudu vkladat stejne klicove slovo 2x
		if(value < ptr->value)
                        return KWInsert(&(ptr->LPtr), key, value);
                else
                        return KWInsert(&(ptr->RPtr), key, value);
	} else {
		ptr = malloc(sizeof(struct keyWord));
		if(ptr != NULL) {
			strcpy(ptr->key, key);
			ptr->value = value;
			ptr->LPtr = NULL;
			ptr->RPtr = NULL;
			*rootPtr = ptr;
			return 1;
		}
		return 0;
	}
}

// smaze tabulku a korektne uvolni misto pro ni alokovane
void KWDispose(tKWPtr *rootPtr) {

	tKWPtr ptr = *rootPtr;
	if(ptr != NULL) {
		KWDispose(&(ptr->LPtr));
		KWDispose(&(ptr->RPtr));
		free(ptr);
		*rootPtr = NULL;
	}
}

// vytvori tabulku klicovcyh slov (jiz obsahujici vybrana klicova slova)
// vraci ukazatel na vytvorenou tabulku
// pokud neni dostatek mista pro vytvoreni tabulky, vraci NULL
//    toto je potreba po volani funkce kontrolovat
tKWPtr KWInit() {

	int error = 0;
	char *keys[9] = {"if", "for", "else", "float64", "func", "package", "string", "return", "int"};
	tokenType values[9] = {KW_IF, KW_FOR, KW_ELSE, KW_FLOAT64, KW_FUNC, KW_PACKAGE, KW_STRING, KW_RETURN, KW_INT};
	tKWPtr ptr = NULL;
	for(int i = 0; i < 9; i++)
		if(KWInsert(&ptr, keys[i], values[i]) == 0)
			error = 1;
	if(error == 1) {
		KWDispose(&ptr);
		fprintf(stderr, "Chyba: Nepodarilo se vytvorit tabulku klicovych slov (nedostatek volne pameti)\n");
	}
	return ptr;
}

// podle klice 'key' vyhleda v tabulce prislusne klicove slovo
//    pri uspesnem vyhledani vraci hodnotu 'value' daneho klicoveho slova, coz je cislo ve vyctu
//    pri neuspechu vraci UNKNOWN
tokenType KWLookUp(tKWPtr rootPtr, char *key) {

	if(rootPtr == NULL)
		return UNKNOWN;
	else {
		int rel = strcmp(key, rootPtr->key);
		if(rel == 0)
			return rootPtr->value;
		else {
			if(rel < 0)
				return KWLookUp(rootPtr->LPtr, key);
			else
				return KWLookUp(rootPtr->RPtr, key);
		}
	}
}
