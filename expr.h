#ifndef EXPR_H
#define EXPR_H

// rozhrani pro zpracovani vyrazu
#include "project.h"
#include "symtable.h"

// struktura pro token jako clena dvojsmerne vazaneho linearniho seznamu
typedef struct tokenListItem{
	tToken  token;
	bool    startOfExpr;
	bool    term;
	varType type;
	int     frameNumber;
	struct  tokenListItem *next;
	struct  tokenListItem *prev;
}*tokenListItemPtr;

// dvojsmerne vazany linearni seznam tokenu
typedef struct tokenList{
	tokenListItemPtr first;
	tokenListItemPtr active;
	tokenListItemPtr last;
	tokenListItemPtr lastTerm;
	tokenListItemPtr startOfExpr;
}*tokenListPtr;

// ===============================================================
// Vytvoreni seznamu
//
// inicializace seznamu tokenu
void tokenListInit(tokenListPtr ptr);

// pridani tokenu na konec seznamu
//    prvni token pridany do seznamu se automaticky stava aktivnim
//    pridava-li do seznamu token ID nebo literal, nastavuje term=false
//
// funkce je pouzita i pro prikaz prirazeni a to nasledovne:
//    tvorim-li seznam na leve strane, volam funkci s danym tokenem a tabulkou symbolu => result=NULL
//   
//    tvorim-li seznam na prave strane, volam funkci s vysledkem precedencni analyzy (result), STab = OPT
//    tvorim-li seznam pro precedencni analyzu volam pouze s tokenem => STab = OPT, result = NULL
int tokenAppend(tokenListPtr ptr, tToken* token, tSymTablePtr STab, tokenListItemPtr result);

// zruseni seznamu
void tokenListDispose(tokenListPtr ptr);


// ==========================================================================
// Prikaz vicenasobneho prirazeni se semantickou kontrolou a generovanim kodu
//
// provede semantickou kontrolu pro prikaz prirazeni a vygeneruje odpovidajici instrukce
//    dest je leva strana prirazeni
//    src je prava strana prirazeni
int tokenListAssign(tokenListPtr dest, tokenListPtr src);

// provadi vypisy instrukci pro parametry pri volani funkce
int tokenParamHandler(tSymTablePtr STab, tToken *token, tokenListPtr ptr);

// vypisuje instrukce pri volani funkce
void funcCallHandler(tSymTablePtr STab, tokenListPtr ptr);

// zacatek prikazu IF
void handleFalseCond(tSymTablePtr STab, tokenListPtr ptr);

// konec prikazu IF
void handleEndIf(tSymTablePtr STab);

// ===================================================================
// Semanticka kontrola, zda jsou vsechny tokeny stejneho datoveho typu
//
// najde-li id, vyhledava promenne v tabulkach symbolu
// jsou-li vsechny operandy typu string, kontroluje, zda neni pouzita jina operace nez konkatenace
int tokenListSemCheck(tokenListPtr ptr, tSymTablePtr STab);

// semanticke porovnani tokenListu a seznamu navratovych hodnot aktivni funkce
int tokenRetListCompare(tokenListPtr tList, tSymTablePtr STab);

// =============================
// Zpracovani seznamu
//
// posun aktivity na dalsi token
void tokenNext(tokenListPtr ptr);

// na zaklade priority mezi lastTerm a active
//  vklada zacatek podvyrazu do prvku za lastTerm, pokud je NULL, vklada zacatek podvyrazu do prvniho prvku
//  nepovede-li si vlozit zacatek noveho podvyrazu, je nutne zpracovat dosavadni podvyraz
//    nastavuje startOfExpr tokenu
//    vraci nasledujici navratove hodnoty:
//      RET_ERR ... nepovedlo se vlozit startOfExpr tokenu .. nasleduje zpracovani podvyrazu
//      RET_OK  ... povedlo se vlozit startOfExpr tokenu   .. nasleduje prochazeni seznamu
int tokenPrecedence(tokenListPtr ptr);

// zpetne hleda posledni term
void tokenLastTerm(tokenListPtr ptr);

// zpetne hleda zacatek podvyrazu
void tokenStartOfExpr(tokenListPtr ptr);

// nahrazuje podvyraz
//    zrusi cast seznamu a na jeho misto vlozi novy prvek s term=false
//    novy prvku je pomocna promenna
//    pokud je posledni prvek podvyrazu aktivni, posunuje aktivitu na dalsi prvek
//    pokud je prvek posledni v seznamu, je vyraz zpracovan
//    generuje instrukce
int tokenGenerate(tokenListPtr ptr, bool print);

// precedencni analyza pro zpracovani vyrazu
tokenListItemPtr precedence(tokenListPtr ptr, tSymTablePtr STab, bool print);

varType tokenListGetFirstType(tokenListPtr tokenList);

void printConvertString(char* input);

#endif
