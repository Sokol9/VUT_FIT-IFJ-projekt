#ifndef _ERROR_H
#define _ERROR_H

//Error codes
#define RESULTOK		0
#define LEXERROR		1
#define SYNERROR		2
#define SEMDEFERROR		3
#define SEMTYPEDEFERROR		4
#define SEMTYPEERROR		5
#define SEMFUNCERROR		6
#define SEMERROR		7
#define SEMZEROERROR		9
#define INTERNALERROR		99

#endif

//Sets global error
void setError(int Error);
int getError();
