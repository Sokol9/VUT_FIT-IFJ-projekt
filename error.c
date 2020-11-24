#include "error.h"

static int ErrorNum = 0; //actual error num

void SetError(int Error){
  if(ErrorNum == 0){ //if no error was set, sets first error
    ErrorNum = Error;
  }
}

int GetError(){
    return ErrorNum;
}