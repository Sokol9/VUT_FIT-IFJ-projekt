#ifndef ST_PRIVATE
#define ST_PRIVATE

#include "symtable.h"

// Globalni tabulka
//================================================================
//
// private datove typy

// parametr funkce jako prvek linearniho seznamu
struct funcParam{
	char *id;
        varType type;
        struct funcParam *next;
};

// zaznam o funkci v binarnim vyhledavacim strome
struct globalRec{
	bool randomPCount;
	tParamPtr params;
	tRetPtr returns;
	bool defined;
	bool used;
	bool retDefined;
	bool errorFlag;
	struct globalRec *LPtr;
	struct globalRec *RPtr;
	char id[];
};

// private prototypy funkci

// inicializace globalni tabulky symbolu
//    v nove inicializovane tabulce se jiz nachazi zaznam pro povinnou funkci main
//    dale se v ni nachazi vestavene funkce jazyka
void GTInit(tGRPtr *rootPtr);

// vyhledani zaznamu v globalni tabulce
//    vraci ukazatel na nalezenou polozku nebo NULL pri neuspechu
//tGRPtr GTLookUp(tGRPtr rootPtr, char *key);

// zjistuje, zda je funkce jiz definovana nebo ne
//    funkce prijima jako parametr ukazatel na zaznam o teto funkci
bool GTIsDefined(tGRPtr ptr);

// pridani zaznamu do globalni tabulky
//    vraci ukazatel na nove vytvorenou polozku nebo NULL pri neuspechu
//    pomoci parametru define je mozne urcit, zda jde o definici funkce
//    nachazi-li se jiz v tabulce zaznam s priznakem defined=true a volam funkci s define=true dochazi k chybe redefinice funkce
tGRPtr GTInsert(tGRPtr *rootPtr, char *key, bool define);

// prida datovy typ parametru, pokud parametr neexistuje, vytvori ho
//    vraci NULL pokud vytvoreni selze
tParamPtr GTAddParamType(tParamPtr ptr, varType type);

// prida id parametru, pokud parametr neexistuje, vytvori ho
//    vraci NULL pokud vytvoreni selze
tParamPtr GTAddParamId(tParamPtr ptr, char *id);

// prida novy parametr do seznamu parametru funkce, pomocna funkce GTInit
//    ptr je ukazatel na dany zaznam o funkci
int GTAddParam(tGRPtr ptr, varType type, char *id);

// prida novou navratovou hodnotu do seznamu navratovych hodnot funkce
//    ptr je ukazatel na dany zaznam o funkci
int GTAddRet(tGRPtr ptr, varType type);

// smazani globalni tabulky
void GTDispose(tGRPtr *rootPtr);

// Lokalni tabulky
//================================================================
//
// private datove typy

// zaznam o promenne v binarnim vyhledavacim strome
struct localRec{
	varType type;
	//bool used;
	struct localRec *LPtr;
	struct localRec *RPtr;
	char id[];
};

// ramec lokalni tabulky symbolu jako prvek linearniho seznamu
struct localFrame{
        tLRPtr rootPtr;
        struct localFrame *upper;
};

//private prototypy funkci

// vytvoreni a inicializace nove urovne lokalni tabulky symbolu
//    pokud jde o ramec funkce, seznam ramcu zatim neexistuje, volejte tedy funkci s upper=NULL
//    vraci ukazatel na novy zacatek seznamu retezenych ramcu
//    parametrem je ukazatel na dosavadni zacatek seznamu
//    je-li func != NULL vklada do noveho ramce i parametry funkce
tLFPtr LTCreateFrame(tLFPtr upper, tGRPtr func);

// prohledani vsech ramcu lokalni tabulky symbolu
tLRPtr LTSearch(tLFPtr framePtr, char *key);

// vlozeni zaznamu do daneho ramce
//    vraci ukazatel na zaznam o promenne
tLRPtr LTInsert(tLFPtr framePtr, char *key);

// vrati datovy typ promenne dane parametrem ptr
varType LTGetType(tLRPtr ptr);

// zmeni typ promenne, na jejiz zaznam ukazuje ptr
//    pokud neni dosavadni typ promenne UNKNOWN_T nastava chyba
int LTSetType(tLRPtr ptr, varType type);

// zrusi nejvice zanoreny ramec
//    vraci ukazatel na ramec o uroven vyse, NULL pokud byl zrusen nejvrchnejsi ramec
tLFPtr LTDeleteFrame(tLFPtr framePtr);

#endif
