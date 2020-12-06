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
int tokenAppend(tokenListPtr ptr, tToken token);

// zruseni seznamu
void tokenListDispose(tokenListPtr ptr);


// ===================================================================
// Semanticka kontrola, zda jsou vsechny tokeny stejneho datoveho typu
//
// najde-li id, vyhledava promenne v tabulkach symbolu
// jsou-li vsechny operandy typu string, kontroluje, zda neni pouzita jina operace nez konkatenace
int tokenListSemCheck(tokenListPtr ptr, tSymTablePtr);


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

// reset cislovani promennych
void resetNumber();

// nahrazuje podvyraz
//    zrusi cast seznamu a na jeho misto vlozi novy prvek s term=false
//    novy prvku je pomocna promenna
//    pokud je posledni prvek podvyrazu aktivni, posunuje aktivitu na dalsi prvek
//    pokud je prvek posledni v seznamu, je vyraz zpracovan
//    generuje instrukce
int tokenGenerate(tokenListPtr ptr);

// precedencni analyza pro zpracovani vyrazu
tokenListItemPtr precedence(tokenListPtr ptr, tSymTablePtr STab, bool resetVarNumber);

varType tokenListGetFirstType(tokenListPtr tokenList);

#endif
