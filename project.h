#ifndef PROJECT_H
#define PROJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "error.h"
// maximalni delka identifikatoru
#define MAX_LEN 1024
#define KW_LEN 10

// vycet vsech typu tokenu
// operatory, separatory, zavorky, klicova slova, identifikator, literaly, EOF, UNKNOWN
// US(underscore) je specialni promenna '_'
//
// operatory aritmeticke: ADD(+), SUB(-), MULT(*), DIV(/)
// operatory logicke:     LT(<), LTEQ(<=), GT(>), GTEQ(>=), EQ(==), NEQ(!=)
// operatory prirazeni:   ASG(=), DEF(:=)
// separatory:            SEM(;), COM(,)
// zavorky:               OBR('('), CBR(')'), OB('{'), CB('}')
typedef enum {UNKNOWN=0, ADD, SUB, MULT, DIV, SEM, OBR, CBR, OB, CB, LT, LTEQ, GT, GTEQ, EQ, NEQ, ASG, DEF, COM, KW_ELSE, KW_FLOAT64, KW_FOR, KW_FUNC, KW_IF, KW_INT, KW_PACKAGE, KW_RETURN, KW_STRING, ID, US, INT_L, FLOAT_L, STRING_L, TOKEN_EOF} tokenType;

// struktura tToken
// typ urcen lexikalni analyzou, podle typu je rizena syntakticka analyza
// atribut je precteny retezec ze vstupu
// eolFlag urcuje, zda byl pri nacitani znaku zaznamenan konec radku
typedef struct{
	tokenType type;
	char attr[MAX_LEN];
	bool eolFlag;
}tToken;

//================================================================
// binarni vyhledavaci strom pro klicova slova (tabulka klicovych slov)
typedef struct keyWord{
	char key[KW_LEN];
	tokenType value;
	struct keyWord *LPtr;
	struct keyWord *RPtr;
}*tKWPtr;

// funkce obsluhujici praci s tabulkou klicovych slov
//
// inicializace tabulky klicovcyh slov
//    provede vlozeni vsech klicovych slov do tabulky
//    tabulka se inicializuje na zacatku behu programu a dale zustava nemenna
tKWPtr KWInit();

// vlozeni klicoveho slova do tabulky
int KWInsert(tKWPtr *rootPtr, char *key, tokenType value);

// vyhledani klicoveho slova v tabulce
tokenType KWLookUp(tKWPtr rootPtr, char *key);

// zruseni tabulky klicovych slov
void KWDispose(tKWPtr *rootPtr);

//================================================================
// funkce pro vraceni tokenu na pozadani syntaktickeho analyzatoru
// funkce vraci chybovou hodnotu nasledovne:
//    1  ... vse v poradku
//    0  ... lexikalni chyba
// parametry:
//    ukazatel na token, pres ktery funkce token plni hodnotami
//    ukazatel na tabulku klicovych slov
int getToken(tToken *token, tKWPtr table);

//================================================================





#endif
