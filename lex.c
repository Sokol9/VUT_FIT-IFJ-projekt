// externi definice funkce getToken

// project.h ... obsahuje rozhrani pro komunikaci v ramci prekladu (vcetne #include <stdio.h>)
#include "project.h"

int getToken(tToken *token, tKWPtr table) {

	char buffer[MAX_LEN];
	char hex[3];
	for(int i = 0; i < MAX_LEN; i++) {
		buffer[i]      = '\0';
		token->attr[i] = '\0';
	}
	token->type = UNKNOWN;
	token->eolFlag = false;
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
				token->type = DIV;
				return 1;
			}
		}

		// nastaveni eolFlagu
		if(XX == '\n')
			token->eolFlag = true;
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

			//fprintf(stderr, "Chyba: Prilis dlouhy identifikator! [%s]\n Maximalni delka je %d znaku\n", buffer, MAX_LEN-2);
			setError(LEX_ERROR);
			return 0;
		}

		ungetc(XX, stdin);

		if(strcmp(buffer, "_") == 0) {
			token->type = US;
			return 1;
		}

		tokenType kw = KWLookUp(table, buffer);
		if(kw != UNKNOWN)
			token->type = kw;
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
				//fprintf(stderr, "Varovani: Na zacatku cislicoveho literalu nesmi byt prebytecne nuly, cislo bude prijato jako desitkove\n");
				setError(LEX_ERROR);
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

			//fprintf(stderr, "Chyba: Prilis dlouhy celociselny literal\n Pouzivejte prosim, rozumne velka cisla\n");
			setError(LEX_ERROR);
			return 0;
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
					//fprintf(stderr, "Chyba: Prilis dlouhe desetinne cislo\n Pouzivejte prosim rozumna cisla\n");
					setError(LEX_ERROR);
					return 0;
				}

			// neplatna desetinna cast (ani jedna cislice za teckou, zustava default typ INT_L - tecka neni ani v bufferu)
			} else {

				//fprintf(stderr, "Chyba: Neplatna desetinna cast cisla [%s]\n", buffer);
				setError(LEX_ERROR);
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

					//fprintf(stderr, "Chyba: Prilis dlouhy exponent\n Pouzivejte prosim rozumna cisla\n");
					setError(LEX_ERROR);
					return 0;
				}

			// neplatny exponent (ani jedna cislice za e/E (+/-), zustava bud INT_L nebo FLOAT_L podle desetinne casti, exponentova cast neni v bufferu)
			} else {

				//fprintf(stderr, "Chyba: Neplatna exponentova cast cisla [%s]\n", buffer);
				setError(LEX_ERROR);
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
								//fprintf(stderr, "Chyba: Neplatna hexadecimalni cislice v escape sekvenci\n");
								setError(LEX_ERROR);
								ok = 0;
							}
							break;

						default:
							//fprintf(stderr, "Chyba: Nespravna escape sekvence %c%c \n", '\\', (char)XX);
							setError(LEX_ERROR);
							ok = 0;
							break;
					}

				} else if (XX > 31) {
					
					if(XX != '"')
						buffer[buff_index++] = (char)XX;
				} else {

					//fprintf(stderr, "Chyba: Literal na vstupu obsahoval neplatny znak\n");
					setError(LEX_ERROR);
					ok = 0;
				}

			} while(buff_index < MAX_LEN-2 && XX != '"');

			strcpy(token->attr, buffer);
			if(XX != '"') {

				// pri prekroceni velikosti bufferu vrati na stdin '"' pro cteni dalsi casti literalu
				token->attr[MAX_LEN-2] = XX;
				ungetc('"', stdin);
				//fprintf(stderr, "Chyba: Prilis dlouhy retezcovy literal\n");
				setError(LEX_ERROR);
				return 0;
			}

			return ok;

		case '+':
			token->type = ADD;
			return 1;
		case '-':
			token->type = SUB;
			return 1;
		case '*':
			token->type = MULT;
			return 1;
		case ',':
			token->type = COM;
			return 1;
		case '(':
			token->type = OBR;
			return 1;
		case ')':
			token->type = CBR;
			return 1;
		case '{':
			token->type = OB;
			return 1;
		case '}':
			token->type = CB;
			return 1;
		case ';':
			token->type = SEM;
			return 1;
		case '!':
			XX = getchar();
			if(XX == '=') {
				token->type = NEQ;
				return 1;
			}
			ungetc(XX, stdin);
			//fprintf(stderr, "Chyba: Neplatny operator [!]\n");
			setError(LEX_ERROR);
			return 0;
		case ':':
			XX = getchar();
			if(XX == '=') {
				token->type = DEF;
				return 1;
			}
			ungetc(XX, stdin);
			//fprintf(stderr, "Chyba: Neplatny operator [:]\n");
			setError(LEX_ERROR);
			return 0;
		case '=':
			XX = getchar();
			if(XX == '=') {
				token->type = EQ;
				return 1;
			}
			ungetc(XX, stdin);
			token->type = ASG;
			return 1;
		case '<':
			XX = getchar();
			if(XX == '=') {
			        token->type = LTEQ;
				return 1;
			}
			ungetc(XX, stdin);
			token->type = LT;
			return 1;
		case '>':
			XX = getchar();
			if(XX == '=') {
				token->type = GTEQ;
				return 1;
			}
			ungetc(XX, stdin);
			token->type = GT;
			return 1;
		case EOF:
			token->type = TOKEN_EOF;
			return 1;
		default:
			token->type = UNKNOWN;
			//fprintf(stderr, "Chyba: Neplatny znak [%c]\n", (char)XX);
			setError(LEX_ERROR);
			return 0;
	}
}
