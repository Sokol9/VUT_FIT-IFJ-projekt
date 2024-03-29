// hlavickovy soubor pro praci s tabulkami symbolu

#ifndef SYMTABLE_H
#define SYMTABLE_H

// vycet typu pro promenne
typedef enum {UNKNOWN_T=0, INT_T, FLOAT64_T, STRING_T, BOOL_T} varType;

//================================================================
// Globalni tabulka

// parametr funkce jako prvek linearniho seznamu
struct funcParam;
// ukazatel na paramteru funkce
typedef struct funcParam* tParamPtr;

// navratova hodnota jako prvek linearniho seznamu
typedef struct funcRet{
	varType type;
        struct funcRet *next;
}*tRetPtr;

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

// seznam navratovych hodnot
typedef struct RetList{
	tRetPtr first;
	tRetPtr active;
	tRetPtr last;
}*tRetListPtr;

//================================================================
// funcke pro praci se strukturou retList
//
// inicializuje seznam navratovych hodnot
void retListInit(tRetListPtr list);

// vlozi navratovou hodnotu na konec seznamu
int retListInsert(tRetListPtr list, varType type);

// posune aktivitu na dalsi prvek
void retListNext(tRetListPtr list);

// zrusi seznam navratovych hodnot
void retListDispose(tRetListPtr list);

//================================================================
// Funkce pro praci s tabulkami symbolu
//
// inicializace tabulky symbolu
int STInit(tSymTablePtr ptr);

// zjistuje, zda jsou na konci programu vsechny funkce definovane
bool STFuncDefCheck(tGRPtr rootPtr);

// nastaveni aktivity na funkci
int STFuncSetActive(tSymTablePtr ptr, tGRPtr funcPtr);

// pridani nove funkce do tabulky symbolu
//    novou polozku nastavuje na aktivni
//    v pripade chyby ponechava aktivitu na puvodni funkci
int STFuncInsert(tSymTablePtr ptr, char *key, bool define);

// nejdrive zjisti, zda byla funkce jiz drive pouzita
//    pokud ano, provadi kontrolu porovnanim dvou seznamu navratovych hodnot
//    pokud ne, vklada aktivni funkci seznam navratovych hodnot
//    je-li volana s list=NULL, pouze kontroluje, ze funkce nema zadne navratove hodnoty
//
//    seznam navratovych hodnot, ktery bude predan funkci se stava prazdnym
int STFuncInsertParamType(tSymTablePtr ptr, varType type);

// prida aktivnimu parametru id, pokud parametr neexistuje, vytvori novy a nastavi ho jako aktivni
int STFuncInsertParamId(tSymTablePtr ptr, char *id);

// zadana na konci prace s parametry - provadi-li se kontrola, kontroluje, ze je spravny pocet parametru
int STFuncParamEnd(tSymTablePtr ptr);

// nejdrive zjisti, zda byly funkci jiz definovany navratove hodnoty
//    pokud ano, provadi kontrolu porovnanim dvou seznamu navratovych hodnot
//    pokud ne, vklada aktivni funkci seznam navratovych hodnot
//
//    seznam navratovych hodnot, ktery bude predan funkci se stava prazdnym
int STFuncInsertRet(tSymTablePtr ptr, tRetListPtr list);

// vraci nazev aktivni funkce
char* STFuncGetName(tSymTablePtr ptr);

//funkcia vrati ukazatel na aktivnu funkciu
//pouzite v pripade, kedy na chvilu potrebujem zmenit acfivnu funkciu
//a nasledne sa k nej vratit
tGRPtr STGetActiveFunc(tSymTablePtr STab);

//funkcia nastavi ukazatel na aktivnu funkciu
//pouzite v pripade, kedy na chvilu potrebujem zmenit acfivnu funkciu
//a nasledne sa k nej vratit
void STSetActiveFunc(tSymTablePtr STab, tGRPtr func);

// vytvoreni noveho lokalniho ramce
//    pokud je func=true, vytvari novy ramec pro aktivni funkci
int STCreateFrame(tSymTablePtr ptr, bool func);

// ziskani cisla ramce na vrcholu zasobniku
int STGetFrameNumber(tSymTablePtr ptr);

// nastaveni returnFlagu aktivni funkce
void STSetFuncReturn(tSymTablePtr ptr);

// ziskani returnFlagu aktivni funkce
bool STGetFuncReturn(tSymTablePtr ptr);

// vyhledani promenne
//    v pripade nalezeni nastavuje aktivitu na nalezenou promennou
//    v pripade nalezeni take vraci cislo ramce, ve kterem byla promenna nalezena, jinak 0
int STVarLookUp(tSymTablePtr ptr, char *key);

// vlozeni promenne do tabulky symbolu
//    v pripade uspechu nastavuje aktivitu na nove vlozenou prommenou
//    vychozi datovy typ je UNKNOWN
int STVarInsert(tSymTablePtr ptr, char *key);

// zjisti datovy typ aktivni promenne
varType STVarGetType(tSymTablePtr ptr);

// vraci identifikator aktivni promenne
char* STVarGetName(tSymTablePtr ptr);

// zmena datoveho typu aktivni promenne
//    pokud je typ jiny nez UNKNOWN dochazi k chybe
int STVarSetType(tSymTablePtr ptr, varType type);

// smaze ramec na vrcholu zasobniku
int STDeleteFrame(tSymTablePtr ptr);

// odstrani tabulku symbolu
int STDispose(tSymTablePtr ptr);
#endif
