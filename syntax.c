#include "stack.h"
#include <stdio.h>
#include "project.h"

void stackprinter(sStack* S, char** typ){
	for(int i = S->top; i>0; i--){
		printf("%d: %s\n",i, typ[S->s[i]]);
	}
}

void check(grammar nonterm, grammar token, sStack* S){     
	
	char* typ[3] = {"prog", "func", "id"};
	
	stackprinter(S, typ);
	
	if (nonterm != token){
		

	switch (nonterm){

		case prog:
			if (token == id){   //ID jednotna gamatika, nece sa mi
				StackPop(S);
				StackPush(S, ID);
				StackPush(S, ID);
				StackPush(S, ID);
			}else{
				printf("nenasla sa zhoda\n");
			}
			break;
		case func:
			break;
		case id:
			break;			
	}		
	}else{
		StackPop(S);	
	}
	printf("vrchol zasobnika %s\n\n", typ[StackTop(S)]);	
}

