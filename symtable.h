// hlavickovy soubor pro praci s tabulkami symbolu

#ifndef SYMTABLE_H
#define SYMTABLE_H

// vycet typu pro promenne
typedef enum {UNKNOWN_T=0, INT_T, FLOAT64_T, STRING_T} varType;

//================================================================
// Globalni tabulka

// parametr funkce jako prvek linearniho seznamu
struct funcParam;
// ukazatel na paramteru funkce
typedef struct funcParam* tParamPtr;

// navratova hodnota jako prvek linearniho seznamu
struct funcRet;
// ukazatel na navratovou hodnotu funkce
typedef struct funcRet* tRetPtr;

// zaznam o funkci v globalni tabulce symbolu
struct globalRec;
// ukazatel na zaznam o funkci
typedef struct globalRec* tGRPtr;

//================================================================
// Lokalni tabulka

// zaznam o promenne v lokalni tabulce symbolu
struct localRec;
// ukazatel na zaznam o promenne
typedef struct localRec* tLRPtr;

// ramec lokalni tabulky symbolu jako prvek linearniho seznamu
struct localFrame;
// ukazatel na ramec lokalni tabulky symbolu
typedef struct localFrame* tLFPtr;

//================================================================
// Rozhrani pro praci s tabulkami symbolu
//    funkce vraci 0 pri neuspechu, 1 jinak
typedef struct SymTable{
	tGRPtr    rootPtr;
	tGRPtr    activeFunc;
	tParamPtr activeParam;
	tRetPtr   activeRet;
	tLFPtr    topFrame;
	tLRPtr    activeVar;
}*tSymTablePtr;

// inicializace tabulky symbolu
int STInit(tSymTablePtr ptr);

// vyhledani funkce
//    vyhledanou funkci nastavuje na aktivni
//    v pripade chyby ponechava aktivitu na puvodni funkci
//int STFuncLookUp(tSymTablePtr ptr, char *key);

// zjisteni definovani aktivni funkce
bool STFuncIsDefined(tSymTablePtr ptr);

// nastaveni aktivity na funkci
int STFuncSetActive(tSymTablePtr ptr, tGRPtr funcPtr);

// pridani nove funkce do tabulky symbolu
//    novou polozku nastavuje na aktivni
//    v pripade chyby ponechava aktivitu na puvodni funkci
int STFuncInsert(tSymTablePtr ptr, char *key, bool define);

// nejdrive zjisti, zda byla aktivni funkce jiz drive pouzita nebo definovana
//    pokud ano, zkontroluje datovy typ aktivniho parametru a aktivitu posune na dalsi parametr (NULL, kdyz byl kontrolovan posledni)
//    pokud ne, prida aktivnimu parametru datovy typ, pokud parametr neexistuje, vytvori novy a nastavi ho jako aktivni
int STFuncInsertParamType(tSymTablePtr ptr, varType type);

// prida aktivnimu parametru id, pokud parametr neexistuje, vytvori novy a nastavi ho jako aktivni
int STFuncInsertParamId(tSymTablePtr ptr, char *id);

// zadana na konci prace s parametry - provadi-li se kontrola, kontroluje, ze je spravny pocet parametru
int STFuncParamEnd(tSymTablePtr ptr);

// prida novou navratovou hodnotu do seznamu navratovych hodnot aktivni funkce
int STFuncAddRet(tSymTablePtr ptr, varType type);

// vytvoreni noveho lokalniho ramce
//    pokud je func=true, vytvari novy ramec pro aktivni funkci
int STCreateFrame(tSymTablePtr ptr, bool func);

// vyhledani promenne
//    v pripade nalezeni nastavuje aktivitu na nalezenou promennou
int STVarLookUp(tSymTablePtr ptr, char *key);

// vlozeni promenne do tabulky symbolu
//    v pripade uspechu nastavuje aktivitu na nove vlozenou prommenou
//    vychozi datovy typ je UNKNOWN
int STVarInsert(tSymTablePtr ptr, char *key);

// zjisti datovy typ aktivni promenne
varType STVarGetType(tSymTablePtr ptr);

// zmena datoveho typu aktivni promenne
//    pokud je typ jiny nez UNKNOWN dochazi k chybe
int STVarSetType(tSymTablePtr ptr, varType type);

// smaze ramec na vrcholu zasobniku
int STDeleteFrame(tSymTablePtr ptr);

// odstrani tabulku symbolu
int STDispose(tSymTablePtr ptr);
#endif
