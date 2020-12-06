// ===================
// Precedencni analyza

// makro pro nastaveni nove pomocne promenne v ramci aritmetickych operaci
#define VAR_ARITH() \
	middle->token.type = ID; \
	middle->type = end->type

// makro pro nastaveni nove pomocne promenne v ramci logickych operaci
#define VAR_BOOL() \
	middle->token.type = ID; \
	middle->type = BOOL_T

// makro pro generovani instrukce definice nove pomocne promenne
#define DEFVAR_PREC(var) \
	printf("DEFVAR %s\n", var)

// makro pro generovani jednoduchych trojadresnych aritmetickych nebo logickych instrukci
#define INST_PREC(inst, target, f1, var1, f2, var2) \
	printf("%s %s %s%s %s%s\n", inst, target, (f1==-1)?"":((f1==0)?constPrefixes[(int)start->type]:pref1), var1, (f2==-1)?"":((f2==0)?constPrefixes[(int)end->type]:pref2), var2)

// ===================
// Syntaxe a Semantika

// instrukce pro definici aktivni promenne v aktivnim bloku
#define DEFVAR() \
	printf("DEFVAR %s%d$%s\n", "LF@f", STGetFrameNumber(STab), STGetVarName(STab))

// prirazeni hodnoty do promenne
#define ASSIGN(pref1, to, frame, pref2, from) \
	printf("MOVE %s%s %s%s\n", pref1, to, (frame==-1)?"":((frame==0)?constPrefixes[(int)src->active->type]:pref2), from)
