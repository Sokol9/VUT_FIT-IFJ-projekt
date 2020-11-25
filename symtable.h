// hlavickovy soubor pro tabulky symbolu

#ifndef SYMTABLE_H
#define SYMTABLE_H

// vycet typu pro promenne
typedef enum {UNKNOWN_T=0, INT_T, FLOAT64_T, STRING_T} varType;

//================================================================
// Globalni tabulka
//
// struktura pro parametr funkce jako clena linearniho seznamu
typedef struct funcParam{
	varType type;
	struct funcParam *next;
	char id[];
}*tParamPtr;

// struktura pro navratove typy funkce jako cleny linearniho seznamu
typedef struct funcRet{
	varType type;
	struct funcRet *next;
}*tRetPtr;

// struktura pro zaznam o funkci
// prvek globalni tabulky symbolu realizovane binarnim vyhledavacim stromem
typedef struct globalRec{
	bool randomPCount;
	tParamPtr params;
	tRetPtr returns;
	bool defined;
	struct globalRec *LPtr;
	struct globalRec *RPtr;
	char id[];
}*tGRPtr;

// inicializace globalni tabulky symbolu
//    v nove inicializovane tabulce se jiz nachazi zaznam pro povinnou funkci main
//    dale se v ni nachazi vestavene funkce jazyka
tGRPtr GTInit();

// vyhledani zaznamu v globalni tabulce
//    vraci ukazatel na nalezenou polozku nebo NULL pri neuspechu
tGRPtr GTLookUp(tGRPtr rootPtr, char *key);

// zjistuje, zda je funkce jiz definovana nebo ne
//    funkce prijima jako parametr ukazatel na zaznam o teto funkci
bool GTIsDefined(tGRPtr ptr);

// definuje polozku, na niz ukazuje ptr
void GTDefine(tGRPtr ptr);

// pridani zaznamu do globalni tabulky
//    vraci ukazatel na nove vytvorenou polozku nebo NULL pri neuspechu
//    pomoci parametru define je mozne urcit, zda jde o definici funkce
//    nachazi-li se jiz v tabulce zaznam s priznakem defined=true a volam funkci s define=true dochazi k chybe redefinice funkce
tGRPtr GTInsert(tGRPtr *rootPtr, char *key, bool define);

// prida novy parametr do seznamu parametru funkce
//    ptr je ukazatel na dany zaznam o funkci
int GTAddParam(tGRPtr ptr, varType type, char *id);

// prida novou navratovou hodnotu do seznamu navratovych hodnot funkce
//    ptr je ukazatel na dany zaznam o funkci
int GTAddRet(tGRPtr ptr, varType type);

// smazani globalni tabulky
void GTDispose(tGRPtr *rootPtr);

//================================================================
// Lokalni tabulka
//
// struktura pro zaznam o promenne
// prvek lokalni tabulky symbolu realizovane binarnim vyhledavacim stromem
typedef struct localRec{
	varType type;
	//bool used;
	struct localRec *LPtr;
	struct localRec *RPtr;
	char id[];
}*tLRPtr;

// linearni seznam s jednotlivymi lokalnimi tabulkami
//    vzajemne provazane lokalni ramce
typedef struct localFrame{
	tLRPtr rootPtr;
	struct localFrame *upper;
}*tLFPtr;

// vytvoreni a inicializace nove urovne lokalni tabulky symbolu
//    pokud jde o ramec funkce, seznam ramcu zatim neexistuje, volejte tedy funkci s upper=NULL
//    vraci ukazatel na novy zacatek seznamu retezenych ramcu
//    parametrem je ukazatel na dosavadni zacatek seznamu
//    je-li func != NULL vklada do noveho ramce i parametry funkce
tLFPtr LTCreateFrame(tLFPtr upper, tGRPtr func);

// vyhledani zaznamu
//    pomocna funkce pro LTSearch
//    rekurzivni implementace prohledani BVS
//    vraci ukazatel na zaznam o promenne nebo NULL pokud zaznam neexistuje
tLRPtr LTLookUp(tLRPtr framePtr, char *key);

// prohledani ramce/ramcu lokalni tabulky symbolu
//    parametr searchAll udava, zda se ma hledat pouze v nejzanorenejsim ramci nebo ve vsech
tLRPtr LTSearch(tLFPtr framePtr, char *key, bool searchAll);

// pomocna funkce pro LTInsert, vlozeni zaznamu do stromu
//   vraci ukazatel na nove vytvoreny zaznam nebo NULL, pokud se ho nepodarilo vytvorit
//   pokus o vlozeni zaznamu s identifikatorem, ktery se jiz ve stromu nachazi, vyusti v chybu redefinice promenne
tLRPtr LTInsertToTree(tLRPtr *rootPtr, char *key);

// vlozeni zaznamu do daneho ramce
//    vraci ukazatel na zaznam o promenne
tLRPtr LTInsert(tLFPtr framePtr, char *key);

// zmeni typ promenne, na jejiz zaznam ukazuje ptr
//    pokud neni dosavadni typ promenne UNKNOWN_T nastava chyba
int LTAddType(tLRPtr ptr, varType type);

// pomocna funkce provadejici smazani binarniho stromu
void LTDeleteTree (tLRPtr *rootPtr);

// zrusi nejvice zanoreny ramec
//    vraci ukazatel na ramec o uroven vyse, NULL pokud byl zrusen nejvrchnejsi ramec
tLFPtr LTDeleteFrame(tLFPtr framePtr);
#endif
