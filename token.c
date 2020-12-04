#include <string.h>
#include "project.h"

void saveToken(tToken* token){
	strcpy(token->oldAttr, token->attr);
	token->oldType = token->type;
}
