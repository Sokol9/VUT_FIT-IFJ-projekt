#include "stack.h"
#include <stdio.h>
#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"

#include "project.h"


void check(grammar nonterm, grammar token, sStack* S){     

	if (nonterm != token){
	

	switch (nonterm){

		case prog:
			if (token == id){   //ID jednotna gamatika, nece sa mi
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
	printf("vrchol zasobnika %d\n", StackTop(S));	
}

