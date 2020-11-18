//dopln error hendling

#include "stack.h"
#include <stdio.h>

void StackInit(sStack* S){
	S->top = 0;
	StackPush(S, prog);
}

void StackPush(sStack* S, grammar word){
	if (S->top == MAXSTACK-1){
		printf("stack is full\n"); //chyba 99
	}else{
		S->s[S->top++] = word;
	}
}

grammar StackPop(sStack* S){
	if (S->top == 0){
		printf("stack is empty\n");  //chyba99
		return 0;
	}else{
		return S->s[S->top--];
	}
}


grammar StackTop(sStack* S){
   if (S->top == 0){
        printf("stack is empty\n");	//chyba99
		return 0;
    }else{
        return S->s[S->top];
    }
}

