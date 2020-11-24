#include "error.h"

static int ErrorNum = 0; //actual error num

void setError(int Error){
  if(!ErrorNum){ //if no error was set, sets first error
    ErrorNum = Error;
  }
}

int getError(){
    return ErrorNum;
}
