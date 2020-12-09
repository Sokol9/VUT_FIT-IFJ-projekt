// ===================
// Vypis operandu
#define PRINT_OPERAND(type, frame, operand)					\
	if(type == STRING_L) printConvertString(operand);			\
	else if(type == FLOAT_L) printf(" float@%a", strtod(operand, endptr));	\
	else if(type == INT_L) printf(" int@%s",operand);			\
	else if(frame == -1) printf(" %s", operand);				\
	else printf(" LF@f%d$%s", frame, operand)

#define NEWLINE() putchar('\n');

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

// makro pro generovani instrukci
#define INST(inst) \
	printf("%s", inst)

// ===================
// Syntaxe a Semantika

// zacatek programu
#define START() \
	printf(".IFJcode20\nJUMP &main\n");\
	INPUT(I);\
	INPUT(F);\
	INPUT(S);\
	SUBSTR();\
	CHR();\
	ORD()
// inicializace lokalniho ramce pro main
#define INIT_MAIN() \
	printf("CREATEFRAME\nPUSHFRAME\n")

// instrukce vytvoreni navesti
//#define 

// instrukce pro definici aktivni funkce
#define DEFFUNC() \
	printf("LABEL &%s\n", STFuncGetName(STab))
// instrukce pro volani aktivni funkce
#define CALLFUNC() \
	printf("CALL &%s\n", STFuncGetName(STab))
// instrukce pro definici aktivni promenne v aktivnim bloku
#define DEFVAR() \
	printf("DEFVAR LF@f%d$%s\n", STGetFrameNumber(STab), STVarGetName(STab))

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
#define PUSHPARAM(number) \
	printf("MOVE TF@%%%d", number)

// vytvori navratovou hodnotu
#define DEFRET(number) \
	printf("DEFVAR TF@%%retval%d\n", number)
// vrati navratovou hodnotu
#define POPRET(frame, dest, number) \
	printf("MOVE LF@f%d$%s TF@%%retval%d\n", frame, dest, number)

// instrukce pro vstup
#define I 0 
#define F 1
#define S 2
#define INPUT(t)										\
	printf("LABEL &input%c\n", 		    (t==I)?'i':((t==F)?'f':'s'));		\
	printf("DEFVAR LF@check\n");								\
	printf("DEFVAR LF@cond\n");								\
	printf("READ LF@%%retval1 %s\n",	    (t==I)?"int":((t==F)?"float":"string"));	\
	printf("TYPE LF@check LF@%%retval1\n");							\
	printf("EQ LF@cond LF@check string@%s\n",   (t==I)?"int":((t==F)?"float":"string"));	\
	printf("JUMPIFNEQ &input_chyba LF@cond bool@true\n");	\
	printf("MOVE LF@%%retval2 int@0\n");			\
	printf("JUMP &input_end\n");				\
	printf("LABEL &input_chyba\n");				\
	printf("MOVE LF@%%retval1 nil@nil\n");			\
	printf("MOVE LF@%%retval2 int@1\n");			\
	printf("LABEL &input_end\n");				\
	printf("RETURN\n")
	
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
	printf("LABEL &substr_nastav_n\n");			\
	printf("MOVE LF@%%3 LF@tmp\n");				\
	printf("JUMP &substr_pokracuj\n\n");			\
	printf("LABEL &substr_chyba\n");			\
	printf("MOVE LF@%%retval1 string@\n");			\
	printf("MOVE LF@%%retval2 int@1\n");			\
	printf("JUMP &substr_end\n\n");				\
	printf("LABEL &substr_pokracuj\n");			\
	printf("DEFVAR LF@count\n");				\
	printf("MOVE LF@count int@0\n");			\
	printf("DEFVAR LF@substring\n");			\
	printf("MOVE LF@substring string@\n");			\
	printf("DEFVAR LF@char\n");				\
	printf("ADD LF@%%3 LF@%%3 LF@%%2\n\n");			\
	printf("LABEL &substr_for_start\n");			\
	printf("LT LF@cond LF@count LF@%%2\n");			\
	printf("JUMPIFNEQ &substr_for_end LF@cond bool@true\n");\
	printf("ADD LF@count LF@count int@1\n");		\
	printf("JUMP &substr_for_start\n");			\
	printf("LABEL &substr_for_end\n\n");			\
	printf("LABEL &substr_start_for\n");			\
	printf("LT LF@cond LF@count LF@%%3\n");			\
	printf("JUMPIFNEQ &substr_end_for LF@cond bool@true\n");\
	printf("GETCHAR LF@char LF@%%1 LF@count\n");		\
	printf("CONCAT LF@substring LF@substring LF@char\n");	\
	printf("ADD LF@count LF@count int@1\n");		\
	printf("JUMP &substr_start_for\n");			\
	printf("LABEL &substr_end_for\n\n");			\
	printf("MOVE LF@%%retval1 LF@substring\n\n");		\
	printf("LABEL &substr_end\n");				\
	printf("RETURN\n")					\

// instrukce CHR
#define CHR()							\
	printf("LABEL &chr\n");					\
	printf("MOVE LF@%%retval2 int@0\n");			\
	printf("DEFVAR LF@cond\n");				\
	printf("GT LF@cond LF@%%1 int@255\n");			\
	printf("JUMPIFEQ &chr_error LF@cond bool@true\n");	\
	printf("LT LF@cond LF@%%1 int@0\n");			\
	printf("JUMPIFEQ &chr_error LF@cond bool@true\n");	\
	printf("INT2CHAR LF@%%retval1 LF@%%1\n");		\
	printf("JUMP &chr_end\n");				\
	printf("LABEL &chr_error\n");				\
	printf("MOVE LF@%%retval1 nil@nil\n");			\
	printf("MOVE LF@%%retval2 int@1\n");			\
	printf("LABEL &chr_end\n");				\
	printf("RETURN\n")					

// instrukce ORD
#define ORD()							\
	printf("LABEL &ord\n");					\
	printf("DEFVAR LF@cond\n");				\
	printf("DEFVAR LF@char\n");				\
	printf("DEFVAR LF@length\n");				\
	printf("STRLEN LF@length LF@%%1\n");			\
	printf("SUB LF@length LF@length int@1\n");		\
	printf("LT LF@cond LF@%%2 int@0\n");			\
	printf("JUMPIFEQ &ord_chyba LF@cond bool@true\n");	\
	printf("GT LF@cond LF@%%2 LF@length\n");		\
	printf("JUMPIFEQ &ord_chyba LF@cond bool@true\n");	\
	printf("GETCHAR LF@char LF@%%1 LF@%%2\n");		\
	printf("MOVE LF@%%retval1 LF@char\n");			\
	printf("MOVE LF@%%retval2 int@0\n");			\
	printf("JUMP &ord_end\n");				\
	printf("LABEL &ord_chyba\n");				\
	printf("MOVE LF@%%retval1 nil@nil\n");			\
	printf("MOVE LF@%%retval2 int@1\n");			\
	printf("LABEL &ord_end\n");				\
	printf("RETURN\n")	

