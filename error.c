#include "error.h"
#include <stdio.h>

static int ErrorNum = 0; //actual error num

void setError(int Error){
//	printf("iam setting error\n");
  if(!ErrorNum){ //if no error was set, sets first error
    ErrorNum = Error;
  }
}

int getError(){
    return ErrorNum;
}
