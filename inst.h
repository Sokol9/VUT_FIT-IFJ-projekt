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
	printf("%s %s %s%s %s%s\n", inst, target, (f1 == -1)? "" : pref1, var1, (f2 == -1)? "" : pref2, var2)

// definice promenne v bloku
#define DEFVAR() \
	printf("DEFVAR %s\n")
