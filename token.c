#include <string.h>
#include "project.h"

void saveToken(tToken* token){
	strcpy(token->savedToken->attr, token->attr);
	token->savedToken->type = token->type;
}

void tokenInit(tToken* token){
	tToken* savedToken = malloc(sizeof(tToken));
	if (savedToken == NULL) setError(INTERNAL_ERROR);
	savedToken->savedToken = NULL;
	token->savedToken = savedToken;
}

void tokenDispose(tToken* token){
	tToken* savedToken = token->savedToken;
	while(savedToken != NULL){
		free(token);
		token = savedToken;
		savedToken = token->savedToken;
		
	}
	free(token);
}


