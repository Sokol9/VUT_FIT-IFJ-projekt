#ifndef STACK_H
#define STACK_H

#define MAXSTACK 1024



typedef enum {
		prog, func, id
	}grammar;

typedef struct{
	int top;
	grammar s[MAXSTACK];
}sStack;



void StackInit(sStack* S);
void StackPush(sStack* S, grammar word);
grammar StackPop(sStack* S);
grammar StackTop(sStack*S);

#endif //STACK_H
