#ifndef PROJECT_H
#define PROJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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
typedef struct{
	tokenType type;
	char attr[MAX_LEN];
}tToken;

// vycet kriterii pro vyskyt EOLu pri lexikalni analyze
// FORB ... EOL je zakazan
// REQ  ... EOL je vyzadovan
// OPT  ... EOL je volitelny
typedef enum {FORB, REQ, OPT} eolFlag;

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
//    0  ... lexikalni chyba, ale token je "validni" v ramci moznosti
//    -1 ... token nevalidni, neocekavane odradkovani
//    -2 ... token nevalidni, prilis dlouhy identifikator
// parametry:
//    ukazatel na token, pres ktery funkce token plni hodnotami
//    ukazatel na eolFlag (v pripade, ze je REQ a funkce EOL precte na vstupu, meni ho na OPT)
//    ukazatel na tabulku klicovych slov
int getToken(tToken *token, eolFlag *ef, tKWPtr table);


//================================================================
void rule_prog(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_func_def(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_params(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_return_type(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_body(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_stat(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_term(tToken *token, bool* sucess);

void rule_term_n(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_func_call(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_func_def(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_var_def(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_expr(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);

void rule_op(tToken *token, bool* sucess);
void rule_expr_n(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_id_n(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_var_asg(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_values(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_if(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_expr_bool(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_bool_op(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_else(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_for(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_params(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_params_n(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_type(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_type_n(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_return_type(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);
void rule_func_n(tToken *token, tKWPtr keyWords, eolFlag *ef, bool* sucess);






#endif
