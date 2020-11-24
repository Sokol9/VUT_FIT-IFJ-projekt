#ifndef _ERROR_H
#define _ERROR_H

//Error codes
#define ResultOK		0
#define LexError		1
#define SynError		2
#define SemDefError		3
#define SemTypeDefError		4
#define SemTypeError		5
#define SemFuncError		6
#define SemError		7
#define SemZeroError		9
#define InternalError		99

#endif

//Sets global error
void SetError(int Error);
int GetError();