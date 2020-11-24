#ifndef _ERROR_H
#define _ERROR_H

//Error codes
#define RESULT_OK		0
#define LEX_ERROR		1
#define SYN_ERROR		2
#define SEM_DEF_ERROR		3
#define SEM_TYPEDEF_ERROR	4
#define SEM_TYPE_ERROR		5
#define SEM_FUNC_ERROR		6
#define SEM_ERROR		7
#define SEM_ZERO_ERROR		9
#define INTERNAL_ERROR		99

#endif

//Sets global error
void setError(int Error);
int getError();
