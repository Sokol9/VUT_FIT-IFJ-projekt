#include <stdio.h>

// maximalni delka identifikatoru
#define MAX_LEN 1024

// vycet vsech typu tokenu
// operator, klicove slovo, identifikator, literal, EOF
typedef enum {OP, KW, ID, INT_L, FLOAT_L, STRING_L, TOKEN_EOF} tokenType;

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

// funkce pro vraceni tokenu na pozadani syntaktickeho analyzatoru
// funkce vraci chybovou hodnotu nasledovne:
//    1  ... vse v poradku
//    0  ... lexikalni chyba, ale token je "validni" v ramci moznosti
//    -1 ... token nevalidni, neocekavane odradkovani
//    -2 ... token nevalidni, prilis dlouhy identifikator
// parametry:
//    ukazatel na token, pres ktery funkce token plni hodnotami
//    ukazatel na eolFlag (v pripade, ze je REQ a funkce EOL precte na vstupu, meni ho na OPT)
int getToken(tToken *token, eolFlag *ef);



