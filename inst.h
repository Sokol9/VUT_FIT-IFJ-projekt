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

// zacatek programu
#define START() \
	printf(".IFJcode20\nJUMP &main\n")
// inicializace lokalniho ramce pro main
#define INIT_MAIN() \
	printf("CREATEFRAME\nPUSHFRAME\n")

// instrukce pro definici aktivni funkce
#define DEFFUNC() \
	printf("LABEL &%s\n", STFuncGetName(STab))
// instrukce pro volani aktivni funkce
#define CALLFUNC() \
	printf("CALL &%s\n", STFuncGetName(STab))
// instrukce pro definici aktivni promenne v aktivnim bloku
#define DEFVAR() \
	printf("DEFVAR LF@f%d$%s\n", STGetFrameNumber(STab), STGetVarName(STab))

// vytvori novy docasny ramec pri volani funkce
#define CREATEFRAME() \
	printf("CREATEFRAME\n")
// vlozi docasny ramec na vrchol zasobniku ramcu
#define PUSHFRAME() \
	printf("PUSHFRAME\n")
// vyjme vrchol zasobniku ramcu a vlozi ho do docasneho ramce
#define POPFRAME() \
	printf("POPFRAME\n")

// vytvori parametr
#define DEFPARAM(number) \
	printf("DEFVAR TF@%%%d\n", number)
// preda parametr hodnotou
#define PUSHPARAM(number, prefix, value) \
	printf("MOVE TF@%%%d %s%s\n", number, (frame > 0)? prefix : constPrefixes[(int)type], value)

// vytvori navratovou hodnotu
#define DEFRET(num) \
	printf("DEFVAR TF@%%retval%d\n", num)
// vrati navratovou hodnotu
#define POPRET(frame, dest, number) \
	printf("MOVE LF@f%d$%s TF@%%retval%d\n", frame, dest, number)

// prirazeni hodnoty do promenne
#define ASSIGN(pref1, to, frame, pref2, from) \
	printf("MOVE %s%s %s%s\n", pref1, to, (frame==-1)?"":((frame==0)?constPrefixes[(int)src->active->type]:pref2), from)

// instrukce WRITE
#define WRITE() \
	printf("WRITE %s%s\n", (frame > 0)? prefix : constPrefixes[(int)type], token->attr)

// instrukce INT2FLOAT a FLOAT2INT
#define CONV_NUMBER(i2f, destFrame, dest, src) \
	printf("%s LF@f%d$%s %s%s\n", (i2f)? "INT2FLOAT" : "FLOAT2INT", destFrame, dest, (frame > 0)? prefix : constPrefixes[(int)type], src)

// instrukce LEN
#define LEN(destFrame, dest, src) \
	printf("LEN LF@f%d$%s %s%s\n", destFrame, dest, (frame > 0)? prefix : constPrefixes[(int)type], src)

// instrukce SUBSTR
#define SUBSTR() \
	printf("LABEL &substr\n");				\
	printf("DEFVAR LF@cond\n");				\
	printf("DEFVAR LF@tmp\n");				\
	printf("MOVE LF@%%retval2 int@0\n");			\
	printf("STRLEN LF@tmp LF@%%1\n");			\
	printf("LT LF@cond LF@%%3 int@0\n");			\
	printf("JUMPIFEQ &substr_chyba LF@cond bool@true\n");	\
	printf("LT LF@cond LF@%%2 int@0\n");			\
	printf("JUMPIFEQ &substr_chyba LF@cond bool@true\n");	\
	printf("GT LF@cond LF@%%2 LF@tmp\n");			\
	printf("JUMPIFEQ &substr_chyba LF@cond bool@true\n");	\
	printf("SUB LF@tmp LF@tmp LF@%%2\n");			\
	printf("GT LF@cond LF@%%3 LF@tmp\n");			\
	printf("JUMPIFEQ &substr_nastav_n LF@cond bool@true\n");\
	printf("JUMP &substr_pokracuj\n\n");			\
								\
	printf("LABEL &substr_nastav_n\n");			\
	printf("MOVE LF@%%3 LF@tmp\n");				\
	printf("JUMP &substr_pokracuj\n\n");			\
								\
	printf("LABEL &substr_chyba\n");			\
	printf("MOVE LF@%%retval1 string@\n");			\
	printf("MOVE LF@%%retval2 int@1\n");			\
	printf("JUMP &substr_end\n\n");				\
								\
	printf("LABEL &substr_pokracuj\n");			\
	printf("DEFVAR LF@count\n");				\
	printf("MOVE LF@count int@0\n");			\
	printf("DEFVAR LF@substring\n");			\
	printf("MOVE LF@substring string@\n");			\
	printf("DEFVAR LF@char\n");				\
	printf("ADD LF@%%3 LF@%%3 LF@%%2\n\n");			\
								\
	printf("LABEL &substr_for_start\n");			\
	printf("LT LF@cond LF@count LF@%%2\n");			\
	printf("JUMPIFNEQ &substr_for_end LF@cond bool@true\n");\
	printf("ADD LF@count LF@count int@1\n");		\
	printf("JUMP &substr_for_start\n");			\
	printf("LABEL &substr_for_end\n\n");			\
								\
	printf("LABEL &substr_start_for\n");			\
	printf("LT LF@cond LF@count LF@%%3\n");			\
	printf("JUMPIFNEQ &substr_end_for LF@cond bool@true\n");\
	printf("GETCHAR LF@char LF@%%1 LF@count\n");		\
	printf("CONCAT LF@substring LF@substring LF@char\n");	\
	printf("ADD LF@count LF@count int@1\n");		\
	printf("JUMP &substr_start_for\n");			\
	printf("LABEL &substr_end_for\n\n");			\
								\
	printf("MOVE LF@%%retval1 LF@substring\n\n");		\
								\
	printf("LABEL &substr_end\n");				\
	printf("RETURN\n")					\

#define INPUT(type)   /*** TODO ***/ //variants: int, float, string
#define ORD           /*** TODO ***/
#define CHR           /*** TODO ***/






