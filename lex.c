// externi definice funkce getToken

#include "project.h"
#include "error.h"

// navratove hodnoty
#define RET_ERR 0
#define RET_OK  1

int getToken(tToken *token, tKWPtr table) {

	for(int i = 0; i < MAX_LEN; i++)
		token->attr[i] = '\0';
	token->type = UNKNOWN;
	token->eolFlag = false;
	int buff_index = 0;
	int XX = getchar();

	// logika pro bile znaky, komentare a eolFlag (vraci token pro deleni)
	while(isspace(XX) || XX == '/') {
		if(XX == '/') {
			XX = getchar();

			// odstraneni jednoradkoveho komentare
			if(XX == '/') {
				while(XX != '\n') {
					XX = getchar();
					if(XX == EOF) {
						token->type = TOKEN_EOF;
						return RET_OK;
					 }
				}			

			// odstraneni viceradkoveho komentare
			} else if(XX == '*') {
				while((XX = getchar()) == '*');
				if(XX != '/') {
					do {
						while(XX != EOF && (XX = getchar()) != '*');
						if(XX == EOF) {
							token->type = TOKEN_EOF;
							setError(LEX_ERROR);
							return RET_OK;
						}
						while((XX = getchar()) == '*');
					} while(XX != '/');
				}

			// operace deleni	
			} else {
				ungetc(XX, stdin);
				token->type = DIV;
				return RET_OK;
			}
		}

		// nastaveni eolFlagu
		if(XX == '\n')
			token->eolFlag = true;
		XX = getchar();
	}

	// ID, klicova slova, specialni promenna '_'
	if(isalpha(XX) || XX == '_') {

		token->type = ID;
		do {
			token->attr[buff_index++] = (char)XX;
			XX = getchar();
		} while((isalnum(XX) || XX == '_') && buff_index < MAX_LEN-1);

		// prilis dlouhy identifikator
		if(isalnum(XX)|| XX == '_') {
			fprintf(stderr, "Chyba: Prilis dlouhy identifikator!\nMaximalni delka je %d znaku\n", MAX_LEN-2);
			setError(LEX_ERROR);
			return RET_ERR;
		}

		ungetc(XX, stdin);

		if(strcmp(token->attr, "_") == 0) {
			token->type = US;
			return RET_OK;
		}

		tokenType kw = KWLookUp(table, token->attr);
		if(kw != UNKNOWN)
			token->type = kw;
		return RET_OK;

	// INT_L nebo FLOAT_L
	} else if(isdigit(XX)) {

		// default, kdyz neni ani desetinna cast, ani exponent je INT_L
		token->type = INT_L;

		// osetreni nuloveho prefixu cisla
		if(XX == '0') {
			XX = getchar();
			if(isdigit(XX)) {
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
			token->attr[buff_index++] = (char)XX;
			XX = getchar();
		} while(buff_index < MAX_LEN-1 && isdigit(XX));

		if(isdigit(XX)) {
			fprintf(stderr, "Chyba: Prilis dlouhy celociselny literal\n Pouzivejte prosim, rozumne velka cisla\n");
			setError(LEX_ERROR);
			return RET_ERR;
		}

		// desetinna cast
		if(XX == '.') {

			XX = getchar();
			// platna desetinna cast
			if(isdigit(XX)) {
				token->type = FLOAT_L;
				if(buff_index < MAX_LEN-2) {
					token->attr[buff_index++] = '.';
					do {
						token->attr[buff_index++] = (char)XX;
						XX = getchar();
					} while(buff_index < MAX_LEN-1 && isdigit(XX));
				}
				if(isdigit(XX)) {
					fprintf(stderr, "Chyba: Prilis dlouhe desetinne cislo\n Pouzivejte prosim rozumna cisla\n");
					setError(LEX_ERROR);
					return RET_ERR;
				}
			// neplatna desetinna cast
			} else
				setError(LEX_ERROR);
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
				while(XX == '0') {
					XX = getchar();
					if(!isdigit(XX)) {
						ungetc(XX, stdin);
						XX = '0';
						break;
					}
				}
				if(buff_index < ((sign == '\0')? MAX_LEN-2 : MAX_LEN-3)) {
					token->attr[buff_index++] = 'e';
					if(sign != '\0')
						token->attr[buff_index++] = sign;
					do {
						token->attr[buff_index++] = (char)XX;
						XX = getchar();
					} while(buff_index < MAX_LEN-1 && isdigit(XX));
				}
				if(isdigit(XX)) {

					fprintf(stderr, "Chyba: Prilis dlouhy exponent\n Pouzivejte prosim rozumna cisla\n");
					setError(LEX_ERROR);
					return RET_ERR;
				}
			// neplatny exponent
			} else
				setError(LEX_ERROR);
		}

		ungetc(XX, stdin);
		return RET_OK;
	}

	// switch pro ostatni varianty, kde muze byt na zacatku retezce pouze jeden znak
	switch(XX) {

		case '"':
			token->type = STRING_L;
			do {
				XX = getchar();
				if(XX == '\\') {
					char hex[3];
					XX = getchar();
					switch(XX) {

						case '"':
							token->attr[buff_index++] = '"';
							XX = '\0';
							break;

						case 'n':
							token->attr[buff_index++] = '\n';
							break;

						case 't':
							token->attr[buff_index++] = '\t';
							break;

						case '\\':
							token->attr[buff_index++] = '\\';
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

								token->attr[buff_index++] = (char)value;
							} else {
								ungetc(XX, stdin);
								XX = '\0';
								setError(LEX_ERROR);
							}
							break;

						default:
							setError(LEX_ERROR);
							if(XX == EOF) {
								token->type = TOKEN_EOF;
								return RET_OK;
							}
							break;
					}
				} else if(XX > 31) {
					if(XX == '"')
						break;
					token->attr[buff_index++] = (char)XX;
				} else {
					setError(LEX_ERROR);
					if(XX == EOF) {
						token->type = TOKEN_EOF;
						return RET_OK;
					}
				}

			} while(buff_index < MAX_LEN-2 && XX != '\n');

			if(XX != '"' && XX != '\n') {
				// pri prekroceni velikosti bufferu vrati na stdin '"' pro cteni dalsi casti literalu
				token->attr[MAX_LEN-2] = XX;
				ungetc('"', stdin);
				fprintf(stderr, "Chyba: Prilis dlouhy retezcovy literal\n");
				setError(LEX_ERROR);
				return RET_ERR;
			}
			return RET_OK;

		case '+':
			token->type = ADD;
			return RET_OK;
		case '-':
			token->type = SUB;
			return RET_OK;
		case '*':
			token->type = MULT;
			return RET_OK;
		case ',':
			token->type = COM;
			return RET_OK;
		case '(':
			token->type = OBR;
			return RET_OK;
		case ')':
			token->type = CBR;
			return RET_OK;
		case '{':
			token->type = OB;
			return RET_OK;
		case '}':
			token->type = CB;
			return RET_OK;
		case ';':
			token->type = SEM;
			return RET_OK;
		case '!':
			XX = getchar();
			if(XX == '=') {
				token->type = NEQ;
				return RET_OK;
			}
			ungetc(XX, stdin);
			setError(LEX_ERROR);
			return RET_ERR;
		case ':':
			XX = getchar();
			if(XX == '=') {
				token->type = DEF;
				return RET_OK;
			}
			ungetc(XX, stdin);
			setError(LEX_ERROR);
			return RET_ERR;
		case '=':
			XX = getchar();
			if(XX == '=') {
				token->type = EQ;
				return RET_OK;
			}
			ungetc(XX, stdin);
			token->type = ASG;
			return RET_OK;
		case '<':
			XX = getchar();
			if(XX == '=') {
			        token->type = LTEQ;
				return RET_OK;
			}
			ungetc(XX, stdin);
			token->type = LT;
			return RET_OK;
		case '>':
			XX = getchar();
			if(XX == '=') {
				token->type = GTEQ;
				return RET_OK;
			}
			ungetc(XX, stdin);
			token->type = GT;
			return RET_OK;
		case EOF:
			token->type = TOKEN_EOF;
			return RET_OK;
		default:
			token->type = UNKNOWN;
			setError(LEX_ERROR);
			return RET_ERR;
	}
}
