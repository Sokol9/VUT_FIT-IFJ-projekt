// externi definice funkce getToken

// ctype.h   ... obsahuje funkce pro klasifikaci  znaku
// string.h  ... obsahuje funkce pro praci s retezci
// project.h ... obsahuje rozhrani pro komunikaci v ramci prekladu (vcetne #include <stdio.h>)
#include <ctype.h>
#include <string.h>
#include "project.h"

int getToken(tToken *token, eolFlag *ef) {

	char buffer[MAX_LEN];
	char hex[3];
	for(int i = 0; i < MAX_LEN; i++) {
		buffer[i]      = '\0';
		token->attr[i] = '\0';
	}
	int buff_index = 0;
	int XX = getchar();
	int ok = 1;

	// logika pro bile znaky, komentare a eolFlag (vraci token pro deleni)
	while(isspace(XX) || XX == '/') {

		if(XX == '/') {

			XX = getchar();

			// odstraneni jednoradkoveho komentare
			if(XX == '/') {

				while(XX != '\n')
					XX = getchar();

			// odstraneni viceradkoveho komentare
			} else if(XX == '*') {

				while((XX = getchar()) == '*');
				if(XX != '/') {

					do {
						while((XX = getchar()) != '*');
						while((XX = getchar()) == '*');

					} while(XX != '/');
				}

			// operace deleni	
			} else {

				ungetc(XX, stdin);
				token->type = OP;
				token->attr[0] = '/';
				return 1;
			}
		}

		// kontrola eolFlagu
		// pro *ef = REQ meni po nacteni EOL na *ef = OPT
		if(XX == '\n') {

			if(*ef == FORB) {
				fprintf(stderr, "Chyba: Neocekavane odradkovani\n");
				return -1;
			}
			*ef = OPT;
		}
		XX = getchar();
	}

	// ID
	if(isalpha(XX) || XX == '_') {

		token->type = ID;

		do {
			buffer[buff_index++] = (char)XX;
			XX = getchar();

		} while((isalnum(XX) || XX == '_') && buff_index < MAX_LEN-1);

		// prilis dlouhy identifikator
		if(isalnum(XX)|| XX == '_') {

			fprintf(stderr, "Chyba: Prilis dlouhy identifikator! [%s]\n Maximalni delka je %d znaku\n", buffer, MAX_LEN-2);
			return -2;
		}

		ungetc(XX, stdin);

		// pro nacteni identifikatoru je potreba jeste kontrola, zda to neni klicove slovo
		// zatim neimplementovana

		strcpy(token->attr, buffer);
		return 1;

	// INT_L nebo FLOAT_L
	} else if(isdigit(XX)) {

		// default, kdyz neni ani desetinna cast, ani exponent je INT_L
		token->type = INT_L;

		// cislice 0 na zacatku je dle zadani neplatna
		if(XX == '0') {
			XX = getchar();
			if(isdigit(XX)) {

				ok = 0;
				fprintf(stderr, "Chyba: Na zacatku cislicoveho literalu nesmi byt prebytecne nuly, cislo bude prijato jako desitkove\n");
				while(XX == '0')
					XX = getchar();

			} else {
				ungetc(XX, stdin);
				XX = '0';
			}
		}

		// celociselna cast
		do {
			buffer[buff_index++] = (char)XX;
			XX = getchar();

		} while(buff_index < MAX_LEN-1 && isdigit(XX));

		if(isdigit(XX)) {

			fprintf(stderr, "Chyba: Prilis dlouhy celociselny literal\n Pouzivejte prosim, rozumne velka cisla\n");
			return -2;
		}

		// desetinna cast
		if(XX == '.') {

			XX = getchar();
			// platna desetinna cast
			if(isdigit(XX)) {

				token->type = FLOAT_L;
				if(buff_index < MAX_LEN-2) {

					buffer[buff_index++] = '.';
					do {
						buffer[buff_index++] = (char)XX;
						XX = getchar();

					} while(buff_index < MAX_LEN-1 && isdigit(XX));
				}

				if(isdigit(XX)) {
					fprintf(stderr, "Chyba: Prilis dlouhe desetinne cislo\n Pouzivejte prosim rozumna cisla\n");
					return -2;
				}

			// neplatna desetinna cast (ani jedna cislice za teckou, zustava default typ INT_L - tecka neni ani v bufferu)
			} else {

				fprintf(stderr, "Chyba: Neplatna desetinna cast cisla [%s]\n", buffer);
				ok = 0;
			}
		}

		// exponent
		if(XX == 'e' || XX == 'E') {

			char sign = '\0';
			XX = getchar();
			if(XX == '+' || XX == '-') {
				sign = (char)XX;
				XX = getchar();
			}

			// platny exponent
			if(isdigit(XX)) {

				token->type = FLOAT_L;

				// ignorovani nul na zacatku exponentu
				while(XX == '0')
					XX = getchar();

				if(buff_index < ((sign == '\0')? MAX_LEN-2 : MAX_LEN-3)) {

					buffer[buff_index++] = 'e';
					if(sign != '\0')
						buffer[buff_index++] = sign;
					do {
						buffer[buff_index++] = (char)XX;
						XX = getchar();

					} while(buff_index < MAX_LEN-1 && isdigit(XX));
				}

				if(isdigit(XX)) {

					fprintf(stderr, "Chyba: Prilis dlouhy exponent\n Pouzivejte prosim rozumna cisla\n");
					return -2;
				}

			// neplatny exponent (ani jedna cislice za e/E (+/-), zustava bud INT_L nebo FLOAT_L podle desetinne casti, exponentova cast neni v bufferu)
			} else {

				fprintf(stderr, "Chyba: Neplatna exponentova cast cisla [%s]\n", buffer);
				ok = 0;
			}
		}

		ungetc(XX, stdin);
		strcpy(token->attr, buffer);
		return ok;

	}

	// switch pro ostatni varianty, kde muze byt na zacatku retezce pouze jeden znak
	switch(XX) {

		case '"':
			token->type = STRING_L;
			do {
				XX = getchar();
				if(XX == '\\') {

					XX = getchar();
					switch(XX) {

						case '"':
							buffer[buff_index++] = '"';
							XX = '\0';
							break;

						case 'n':
							buffer[buff_index++] = '\n';
							break;

						case 't':
							buffer[buff_index++] = '\t';
							break;

						case '\\':
							buffer[buff_index++] = '\\';
							break;

						case 'x':
							hex[0] = hex[1] = hex[2] = '\0';
							XX = getchar();
							if(isxdigit(XX)) {

								hex[0] = (char)XX;
								XX = getchar();
								if(isxdigit(XX))
									hex[1] = (char)XX;
							}
							if(strlen(hex) == 2) {

								int value;
								if(isdigit(hex[0]))
									value = (hex[0] - '0') * 16;
								else if(islower(hex[0]))
									value = (hex[0] - 'a') * 16;
								else
									value = (hex[0] - 'A') * 16;

								if(isdigit(hex[1]))
									value += hex[1] - '0';
								else if(islower(hex[1]))
									value += hex[1] - 'a';
								else
									value += hex[1] - 'A';

								buffer[buff_index++] = (char)value;

							} else {

								ungetc(XX, stdin);
								XX = '\0';
								fprintf(stderr, "Chyba: Neplatna hexadecimalni cislice v escape sekvenci\n");
								ok = 0;
							}
							break;

						default:
							fprintf(stderr, "Chyba: Nespravna escape sekvence %c%c \n", '\\', (char)XX);
							ok = 0;
							break;
					}

				} else if (XX > 31) {
					
					if(XX != '"')
						buffer[buff_index++] = (char)XX;
				} else {

					fprintf(stderr, "Chyba: Literal na vstupu obsahoval neplatny znak\n");
					ok = 0;
				}

			} while(buff_index < MAX_LEN-2 && XX != '"');

			strcpy(token->attr, buffer);
			if(XX != '"') {

				// pri prekroceni velikosti bufferu vrati na stdin '"' pro cteni dalsi casti literalu
				token->attr[MAX_LEN-2] = XX;
				ungetc('"', stdin);
				// fprintf(stderr, "Chyba: Prilis dlouhy retezcovy literal\n");
				return -2;
			}

			return ok;

		case '+':
		case '-':
		case '*':
		case ',':
		case '(':
		case ')':
		case '{':
		case '}':
		case ';':
			token->type = OP;
			token->attr[0] = (char)XX;
			return 1;

		case '!':
		case ':':
			token->type = OP;
			token->attr[0] = (char)XX;
			XX = getchar();
			if(XX == '=') {
				token->attr[1] = (char)XX;
				return 1;
			}
			ungetc(XX, stdin);
			fprintf(stderr, "Chyba: Neplatny operator [%c]\n", token->attr[0]);
			return 0;

		case '=':
		case '<':
		case '>':
			token->type = OP;
			token->attr[0] = (char)XX;
			XX = getchar();
			if(XX == '=') {
				token->attr[1] = (char)XX;
				return 1;
			}
			ungetc(XX, stdin);
			return 1;

		case EOF:
			token->type = TOKEN_EOF;
			return 1;

		default:
			token->type = OP;
			fprintf(stderr, "Chyba: Neplatny znak [%c]\n", (char)XX);
			return 0;
	}
}
