#include <stdio.h>
#include "project.h"
#include "syntax.h"
#include "error.h"
#include "symtable.h"
#include "expr.h"


// <prog> - package main <func_def> <func_n> EOF
//
// <func_n> - epsilon
void rule_prog(tToken *token, tSymTablePtr STab, tKWPtr keyWords,  bool* sucess){
	/*************PACKAGE MAIN*****************/
	do{
		GET_TOKEN
		if (token->type == KW_PACKAGE){
			print_debug("valid KW_PACKAGE")
		}else{
			*sucess = 0;
			printd("KW_PACKAGE")	
			break;
		}

		if (*sucess){
			GET_TOKEN
			EOL_FORBID			
			if (token->type == ID && !strcmp(token->attr, "main" )){
				print_debug("valid ID-main")
				GET_TOKEN
				EOL_REQUIRED
			}else{
				*sucess = 0;
				printd("main")
				break;
			}
		}
	}while(0);
	/**********END OF PACKEGE MAIN *************/

	/**************<FUNC_DEF>*******************/
	rule_func_def(PARAMS);
	EOL_REQUIRED
 	/***********END OF <FUNC_DEF>***************/

	/***************<FUNC_N>********************/
	rule_func_n(PARAMS);
	GET_TOKEN
	/************END OF <FUNC_N>****************/

	/*****************EOF***********************/
	if (token->type == TOKEN_EOF){
		print_debug("valid TOKEN_EOF")
		
		printf("\n===================================================\n\n ANALIZATION FINISH %s\n\n", \
				*sucess ? "SUCESSFULL" : "WITH FAILS");
	}
	/****************END OF EOF*****************/
}


// <func_def> - func id (<params>) <return_types> {<body>}
//
// <params> - epsilon
// <return_types> - epsilon
void rule_func_def(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	CHECK_POINT(type,KW_FUNC)	
	/*****************FUNC**********************/
	do{
		if (!*sucess) break;
	
		if (token->type == KW_FUNC){
			print_debug("valid KW_FUNC") 
		}else{ 
			*sucess = 0;
			printd("KW_FUNC")
			break;
		}
	/*************END OF FUNC*******************/

	/******************ID***********************/
		GET_TOKEN
		EOL_FORBID
		if (token->type == ID){
			print_debug("valid ID")
			//ak nenastal ziadny eror vlozi funkciu do Stab
			//v pripade ze ide o redefiniciu funkcie vola setError
			STFuncInsert(STab, token->attr, true); 
		}else{
			*sucess = 0;
			printd("ID")
			break;
		}
	/**************END OF ID********************/	

	/***************BRACKETS********************/
		GET_TOKEN
		EOL_FORBID
		if (token->type == OBR){
			print_debug("valid (")
		}else{
			*sucess = 0;
			printd("(")
			break;
		}
	/*************END OF BRACKETS***************/
	
	/*****************<PARAMS>******************/
		GET_TOKEN
		//EOL_OPTIONAL -- nie som si isty
		rule_params(PARAMS);
		if (!*sucess) break;
		//vola semanticku kontrolu, ci vsetky doterz nacitane parametre boli spravne
		//v pripade, ze funkcia este nie je definovana, nastavi hodnoty parametrov,
		//pre neskorsiu kontrolu
		STFuncParamEnd(STab);
	/**************END OF <PARAMS>**************/

	/***************BRACKETS********************/
		EOL_FORBID
		if (token->type == CBR){
			print_debug("valid )")
		}else{
			*sucess = 0;
		}
	/*************END OF BRACKETS****************/

	/**************<RETURN TYPES>****************/
		GET_TOKEN
		EOL_FORBID
		rule_return_type(PARAMS);
	/***********END OF <RETURN TYPES>************/

	}while(0);
	
	CHECK_POINT(type,OB)

	//vytvorenie ramca pre semantiku
	STCreateFrame(STab, true);

	/***************BRACKETS********************/
	if (token->type == OB){
		print_debug("valid {")
    }else{
        *sucess = 0;
		printd("{")
		return;
    }
	/*************END OF BRACKETS***************/

	/*****************<BODY>********************/
	GET_TOKEN
	EOL_REQUIRED
	rule_body(PARAMS);
	if (!*sucess) return;
	/*************END OF <BODY>*****************/

	/***************BRACKETS********************/
	if (token->type == CB){
		print_debug("valid }")
		EOL_REQUIRED
		//todo
		//chceck returnFlag in frame
		//ukoncenie ramca pre semantiku
		STDeleteFrame(STab);
	}else{
		*sucess = 0;
		printd("}")
		return;
	}
	/*************END OF BRACKETS***************/	

	GET_TOKEN
}

//<func_n> - <func_def> <func_n>
//
//<func_n> - epsilon
void rule_func_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){

	CHECK_POINT(type,KW_FUNC)
	
	//epsilon
	if (token->type != TOKEN_EOF){
	/*****************<FUNC>********************/
		rule_func_def(PARAMS);
	/**************END OF <FUNC>****************/

	/***************<FUNC_N>********************/	
		EOL_REQUIRED
		rule_func_n(PARAMS);
	/*************END OF <FUNC_N>***************/
	}
}


//<body> - <stat> <body>
//
//<stat> - epsilon
//<body> - epsilon
void rule_body(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	//epsilon
	if (token->type != CB){
	/*****************<STAT>********************/
		rule_stat(PARAMS);
	/**************END OF <STAT>****************/

		CHECK_POINT(eolFlag, true)
		//podmienka, ktora odchyti chybajucu zatvorku (SYNTAX ERROR)
		if (token->type == KW_FUNC || token->type == TOKEN_EOF) return;

	/*****************<BODY>********************/
		EOL_REQUIRED
		rule_body(PARAMS);
	/**************END OF <BODY>****************/
	}
}

//<stat> - <if>
//<stat> - <for>
//<stat> - id <func_call>
//<stat> - id <var_def>
//<stat> - id <var_asg>
//<stat> - return <expr_opt> <expr_n>
//<stat> - epsilon
void rule_stat(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){

	if (token->type == KW_IF){
	/*******************<IF>********************/
		print_debug("valid KW_IF")
		rule_if(PARAMS);
	/***************END OF <IF>*****************/

	}else if (token->type == KW_FOR){
	/******************<FOR>********************/
		print_debug("valid KW_FOR") 
        rule_for(PARAMS);
	/**************WND OF <FOR>*****************/

	}else if (token->type ==  US){
	/**************<VAR_ASG>********************/
		print_debug("valid _")      
		saveToken(token);

		GET_TOKEN
		EOL_FORBID
		rule_var_asg(PARAMS);
	/***********ENF OF <VAR_ASG>****************/

	}else if( token->type == ID){
	/******************ID***********************/
		print_debug("valid ID")      
		saveToken(token);		

		GET_TOKEN
		EOL_FORBID
	/***************END OF ID*******************/

		if (token->type == OBR){
	/**************<FUNC CALL>******************/
			print_debug("valid (")

			GET_TOKEN
	     	rule_func_call(PARAMS);
    /***********END OF <FUNC CALL>***************/

        }else if (token->type == DEF){
	/**************<VAR_DEF>********************/
			print_debug("valid :=")   
			rule_var_def(PARAMS);
	/***********ENF OF <VAR_DEF>****************/

		}else if (token->type == ASG || token->type ==COM){
	/**************<VAR_ASG>********************/
			rule_var_asg(PARAMS);
	/***********ENF OF <VAR_ASG>****************/

		}else{
			*sucess = 0;
			printd("need valid operand")	
		}
	/****************RETURN*********************/
	}else if(token->type == KW_RETURN){
		print_debug("valid KW_RETURN")

		GET_TOKEN
		//podmienka zachytava koniec statementu
		if (!token->eolFlag){
			//vytvorenie zoznamu tokenov pre kontrolu navratovych hodnot
			tokenListPtr tokenListRet = malloc(sizeof(struct tokenList));
			if (tokenListRet) tokenListInit(tokenListRet); else setError(INTERNAL_ERROR);
			
			//pomocny zoznam tokenov pre vyhodnotenie expr
			tokenListPtr tokenListTmp = malloc(sizeof(struct tokenList));
			if (tokenListTmp) tokenListInit(tokenListTmp); else setError(INTERNAL_ERROR);

			rule_expr(PARAMS, tokenListTmp);
			if (*sucess) {
				//precedencna analiza spracuje zoznam tokenov a vysldedny prida
				//do zoznamu urceneho na porovnanie
				if (precedence(tokenListTmp, STab, false) != NULL){
					tokenAppend(tokenListRet, NULL, STab, tokenListTmp->first);
					//toto tu nesmie byt, tokenAppend(tokenListR, NULL, STab, tokenListTmp->first);, vytvori 
					//ukazatel  v R na prvok v TMP, jeden prvok ma teda 2 ukazatele... nakolko v tmp strukture
					//po precedencke neostane nic, co by bolo treba dispostnut, je tu toto zbytocne...
					//tokenListDispose(tokenListTmp);
					free(tokenListTmp);
				}else{
					tokenListDispose(tokenListTmp);
					free(tokenListTmp);	
				}
			}else{
				tokenListDispose(tokenListTmp);
				free(tokenListTmp);
				printf("call dispose\n");
				tokenListDispose(tokenListRet);
				free(tokenListRet);
				return;
			}	
			
			
			rule_expr_n(PARAMS, tokenListRet);
			if (!*sucess){
				
				printf("call dispose\n");
		
				tokenListDispose(tokenListRet);
				free(tokenListRet);
				return;
			}	
			//kontrola ret	
			//todo
			//pockaj na redefinicu
			//STFuncInsertRet(STab, tokenListRet);
			
			printf("chyba\n");
			tokenListDispose(tokenListRet);
			printf("ahojky\n");
			free(tokenListRet);
			if (STIsFuncFrame(STab)) STSetFrameReturn(STab);	
		}
	/*************END OF RETURN******************/
	}else if(token->type != OB){
		//follow(<stat>) == OB (})
		//valid epsilon
		*sucess = 0;
		printd("}")
	}
}

//<func_call> - (<params_actual>)
//
//<params_actual> - epsilon
void rule_func_call(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	STFuncInsert(STab, token->savedToken->attr, false);	

	if (token->type != CBR){
		rule_term(token, STab, sucess, RULE_FCALL);
		if (!*sucess) return;

		GET_TOKEN
		EOL_FORBID
		rule_term_n(PARAMS);
		if (!*sucess) return;
	}
	if (token->type == CBR){
		print_debug("valid )")

	}else{
		*sucess = 0;
		printd(")")
		if (!*sucess) return;
	}
	GET_TOKEN
}

//<term> - id 
//<term> - INT_L
//<term> - FLOAT_L
//<term> - STRING_L
void rule_term(tToken *token, tSymTablePtr STab,  bool* sucess, callAs call){
	if (token->type == ID){
		print_debug("valid ID")
		
		if (STVarLookUp(STab, token->attr)){
			if (call == RULE_FCALL) STFuncInsertParamType(STab, STVarGetType(STab));
		}
	}else if (token->type == INT_L){
		print_debug("valid INT_L")
		if (call == RULE_FCALL) STFuncInsertParamType(STab, INT_T);
	}else if (token->type == FLOAT_L){
		print_debug("valid FLOAT_L")
		if (call == RULE_FCALL) STFuncInsertParamType(STab, FLOAT64_T);
	}else if (token->type == STRING_L){
		print_debug("valid STRING_L")
		if (call == RULE_FCALL) STFuncInsertParamType(STab, STRING_T);
	}else{
		*sucess = 0;
		printd("ID or some value")
	}
}

//<term_n> - , <term> <term_n>
//
//<term_n> - epsilon
void rule_term_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid ,")

		GET_TOKEN
		rule_term(token, STab, sucess, RULE_FCALL);
		if (!*sucess) return;

		GET_TOKEN
		EOL_FORBID
		rule_term_n(PARAMS);
		if (!*sucess) return;
	}
}

//<var_def> -  := <expr>
void rule_var_def(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	// := uz skontrolovan=a
	
	STVarInsert(STab, token->savedToken->attr);
	GET_TOKEN
	//EOL_OPTIONAL - nie som si tym isty
	tokenListPtr tokenList = malloc(sizeof(struct tokenList));
	if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
	rule_expr(PARAMS, tokenList);	
	if (*sucess) {
		if (precedence(tokenList, STab, false) != NULL){
			STVarLookUp(STab, token->savedToken->attr);
			STVarSetType(STab, tokenListGetFirstType(tokenList));
		}
	}
	tokenListDispose(tokenList);	
	free(tokenList);
}

//<expr> - <expr> <op> <expr>/<expr> - ( <expr> ) <expr_continue>
//<expr> - ( <expr> )
//<expr> - <term>
void rule_expr(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tokenListPtr tokenList){
	if (token->type == OBR){
		print_debug("valid (")
		tokenAppend(tokenList, token, STab, NULL);

		GET_TOKEN
		rule_expr(PARAMS, tokenList);
		
		if (!*sucess) return;
		
		if (token->type == CBR){
			print_debug("valid )")
			tokenAppend(tokenList, token, STab, NULL);
			
			EOL_FORBID

		}else{
			*sucess = 0;
			printd(")")
			if (!*sucess) return;
			
		}
		GET_TOKEN
	}else{ 
		rule_term(token, STab, sucess, RULE_EXPR);
		if (!*sucess) return; else tokenAppend(tokenList, token, STab, NULL);
		
		GET_TOKEN
	}


	if (token->type == SUB || token->type == ADD || token->type == MULT ||\
			 token->type == DIV){
		EOL_FORBID
		rule_op(token, sucess);
		if (!*sucess) return; else tokenAppend(tokenList, token, STab, NULL);

		GET_TOKEN
		rule_expr(PARAMS, tokenList);
		if (!*sucess) return;
	}
}

//<op> - +
//<op> - - 
//<op> - *
//<op> - /
void rule_op(tToken *token, bool* sucess){
	if (token->type == SUB){
		print_debug("valid -")
	}else if (token->type == ADD){
		print_debug("valid +")
	}else if (token->type == MULT){
		print_debug("valid *")
	}else if (token->type == DIV){
		print_debug("valid /")
	}else{
		*sucess = 0;
		printd("operand")
	}
}
	
//<var asg> - <id_n> = <values>
void rule_var_asg(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	tokenListPtr tokenListL = malloc(sizeof(struct tokenList));
	if (!tokenListL) setError(INTERNAL_ERROR); else tokenListInit(tokenListL);
	tokenAppend(tokenListL, token->savedToken, STab, NULL);	
		

	rule_id_n(PARAMS, tokenListL);
	if (!*sucess) {
		tokenListDispose(tokenListL);
		free(tokenListL);
		return;
	}

	if (token->type == ASG){
	print_debug("valid =")
	}else{
		*sucess = 0;
		printd("=")
		tokenListDispose(tokenListL);
		free(tokenListL);
		return;
	}

	GET_TOKEN

	rule_values(PARAMS, tokenListL);
	tokenListDispose(tokenListL);
	free(tokenListL);
}

//<values> - <expr> <expr_n>
//<values> - id <func_call>
//<values> - id <op> <expr>    --> <expr> - <expr> <op> <expr>
void rule_values(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tokenListPtr tokenListL){
	if (token->type == OBR || token->type ==INT_L || token->type == FLOAT_L ||\
			token->type == STRING_L){
		
		tokenListPtr tokenListR = malloc(sizeof(struct tokenList));
		if (!tokenListR) setError(INTERNAL_ERROR); else tokenListInit(tokenListR);

		tokenListPtr tokenListTmp = malloc(sizeof(struct tokenList));
		if (tokenListTmp) tokenListInit(tokenListTmp); else setError(INTERNAL_ERROR);	
		rule_expr(PARAMS, tokenListTmp);
		if (!*sucess) {
			tokenListDispose(tokenListR);
			tokenListDispose(tokenListTmp);
			free(tokenListR);
			free(tokenListTmp);
			return;
		}
		if (precedence(tokenListTmp, STab, false) != NULL){	
			tokenAppend(tokenListR, NULL, STab, tokenListTmp->first);	
			//toto tu nesmie byt, tokenAppend(tokenListR, NULL, STab, tokenListTmp->first);, vytvori 
			//ukazatel  v R na prvok v TMP, jeden prvok ma teda 2 ukazatele... nakolko v tmp strukture po 
			//precedencke neostane nic, co by bolo treba dispostnut, je tu toto zbytocne...
			//tokenListDispose(tokenListTmp);
			free(tokenListTmp);
		}else{
			tokenListDispose(tokenListTmp);
			free(tokenListTmp);
		}

		rule_expr_n(PARAMS, tokenListR);
		if (!*sucess){
			tokenListDispose(tokenListR);
			free(tokenListR);
			return;
		}
		//todo
		//retL vs retR check

		tokenListDispose(tokenListR);
		free(tokenListR);


	}else if (token->type == ID){
		print_debug("valid ID")
		saveToken(token);
		
		GET_TOKEN
		EOL_FORBID
		if (token->type == OBR){
			print_debug("valid (")
			//todo 
			//prerobit
			//===kontrola navratovych hodnot===
			//ulozenie aktivnej funkcie
//			tGRPtr actFunc = STGetActiveFunc(STab);
			//activ func == call func
//			STFuncInsert(STab, token->savedToken->attr, false);
			//kontrola navratovych hodnot
//			STFuncInsertRet(STab, retL);
			//nastavenie p;vodnej activ func
//			STSetActiveFunc(STab, actFunc);

			GET_TOKEN
			rule_func_call(PARAMS);
			if (!*sucess) return;

		}else if(token->type == ADD || token->type == SUB || token->type == MULT || \
				token->type == DIV){
			tokenListPtr tokenListTmp = malloc(sizeof(struct tokenList));
			if (tokenListTmp) tokenListInit(tokenListTmp); else setError(INTERNAL_ERROR);
			tokenAppend(tokenListTmp, token->savedToken, STab, NULL);
				
			rule_op(token, sucess);
			if (!*sucess) {
				tokenListDispose(tokenListTmp);
				free(tokenListTmp);
				return;
			}
			tokenAppend(tokenListTmp, token, STab, NULL);

			GET_TOKEN
			rule_expr(PARAMS, tokenListTmp);
			if (!*sucess) {
				tokenListDispose(tokenListTmp);
				free(tokenListTmp);
				return;
			}
			tokenListPtr tokenListR = malloc(sizeof(struct tokenList));
			if (!tokenListR) setError(INTERNAL_ERROR); else tokenListInit(tokenListR);
			
			if (precedence(tokenListTmp, STab, false) != NULL){
				 tokenAppend(tokenListR, NULL, STab, tokenListTmp->first);
			
				//toto tu nesmie byt, tokenAppend(tokenListR, NULL, STab, tokenListTmp->first);, vytvori 
				//ukazatel  v R na prvok v TMP, jeden prvok ma teda 2 ukazatele... nakolko v tmp strukture po 	
				//precedencke neostane nic, co by bolo treba dispostnut, je tu toto zbytocne...
				//tokenListDispose(tokenListTmp);
				free(tokenListTmp);
			}else{
				tokenListDispose(tokenListTmp);
				free(tokenListTmp);
			}		
			
			rule_expr_n(PARAMS, tokenListR);
			if (!*sucess){
				tokenListDispose(tokenListR);
				free(tokenListR);
				return;
			}
			//todo
			//retL vs retR check
			//to aby nekricalo
			
			tokenAppend(tokenListL, token, STab, NULL);
	printf("ahojku\n");
			tokenListDispose(tokenListR);
			free(tokenListR);	
		}else{
			*sucess = 0;
			printd("func call or expr")
		}
	}else{
		*sucess = 0;
		printd("some value")
	}
}

//<expr_n> - , <expr> <expr_n>
//<expr_n> - epsilon
void rule_expr_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tokenListPtr tokenListR){
	if (token->type == COM){
		print_debug("valid ,")
		EOL_FORBID

		GET_TOKEN
		tokenListPtr tokenListTmp = malloc(sizeof(struct tokenList));
		if (tokenListTmp) tokenListInit(tokenListTmp); else setError(INTERNAL_ERROR);
		rule_expr(PARAMS, tokenListTmp);
		if (*sucess) {
			if (precedence(tokenListTmp, STab, false) != NULL){
				tokenAppend(tokenListR, NULL, STab, tokenListTmp->first);
				//toto tu nesmie byt, tokenAppend(tokenListR, NULL, STab, tokenListTmp->first);, vytvori 
				//ukazatel  v R na prvok v TMP, jeden prvok ma teda 2 ukazatele... nakolko v tmp strukture po 	
				//precedencke neostane nic, co by bolo treba dispostnut, je tu toto zbytocne...
				//tokenListDispose(tokenListTmp);
				free(tokenListTmp);
			}else{
				tokenListDispose(tokenListTmp);
				free(tokenListTmp);
				return;
			}	
		}

        rule_expr_n(PARAMS, tokenListR);
		if (!*sucess) return;
	}
}

//<id_n> - , id <id_n> 
//<id_n> - , _ <id_n>
//<id_n> - epsilon
void rule_id_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tokenListPtr tokenListL){
	if (token->type == COM){
		print_debug("valid ,  ")
		EOL_FORBID

		GET_TOKEN
		EOL_FORBID //nie som si isty
		if (token->type == ID || token->type == US){
			tokenAppend(tokenListL, token, STab, NULL);
			print_debug("valid ID or _")
			GET_TOKEN

			rule_id_n(PARAMS, tokenListL);
			if (!*sucess) return;
		}else{
			*sucess = 0;
			printd("ID")
			if (!*sucess) return;
		}
	}
}

// <if> - if <expr_bool> { <body> } <else>
//
// // <body> - epsilon
// <else> - epsilon
void rule_if(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	tokenListPtr tokenList = malloc(sizeof(struct tokenList));
	if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
	do{
		GET_TOKEN
		EOL_FORBID
		
		int numBool = 0;
		rule_expr_bool(PARAMS, tokenList, &numBool);
		if (!*sucess) break;
		
		if (numBool != 1) break;

		if (precedence(tokenList, STab, false) == NULL){
			*sucess=0;
			printd("chyba precedence");
			break;
		}
	}while(0);
	
	tokenListDispose(tokenList);
	free(tokenList);

	CHECK_POINT(type,OB)

	if (token->type == OB){
		print_debug("valid {")
		STCreateFrame(STab, false);
		GET_TOKEN
		EOL_REQUIRED
		rule_body(PARAMS);
		if (!*sucess) return;

		EOL_REQUIRED
		if (token->type == CB){
			print_debug("valid }")

			GET_TOKEN
		}else{
			*sucess = 0;
			printd("}")
			if (!*sucess) return;
		}
	}else{
		*sucess = 0;
		printd("{")
		if (!*sucess) return;
	}
	STDeleteFrame(STab);
	rule_else(PARAMS);
}

// <expr_bool> - <expr> s povolenim bool_op
void rule_expr_bool(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tokenListPtr tokenList, int* numBool){
	if (token->type == OBR){
		print_debug("valid (")
		tokenAppend(tokenList, token, STab, NULL);

		GET_TOKEN
		rule_expr_bool(PARAMS, tokenList, numBool);
		if (!*sucess) return;
		
		if (token->type == CBR){
			print_debug("valid )")
			tokenAppend(tokenList, token, STab, NULL);
			
			EOL_FORBID

		}else{
			*sucess = 0;
			printd(")")
			if (!*sucess) return;
			
		}
		GET_TOKEN
	}else{
		rule_term(token, STab, sucess, RULE_EXPR);
		if (!*sucess) return; else tokenAppend(tokenList, token, STab, NULL);
		
		GET_TOKEN
	}

	if (token->type == SUB || token->type == ADD || token->type == MULT ||\
			 token->type == DIV){
		EOL_FORBID
		rule_op(token, sucess);
		if (!*sucess) return; else tokenAppend(tokenList, token, STab, NULL);

		GET_TOKEN
		rule_expr_bool(PARAMS, tokenList, numBool);
		if (!*sucess) return;
	}else if(token->type == LT || token->type == LTEQ || token->type == GT || token->type == GTEQ ||\
			 token->type == EQ || token->type == NEQ){
		(*numBool)++;
		EOL_FORBID
		rule_bool_op(PARAMS, tokenList);

		rule_expr_bool(PARAMS, tokenList, numBool);
		if (!*sucess) return;
	}
}


//<else> - else { <body> }
//<else> - epsilon
//
//<body> - epsiolon
 void rule_else(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == KW_ELSE)	{
		print_debug("valid KW_ELSE")
		EOL_FORBID

		GET_TOKEN
		EOL_FORBID
		if ( token->type == OB){
			print_debug("valid {")

			GET_TOKEN
			EOL_REQUIRED
			rule_body(PARAMS);
			if (!*sucess) return;

			EOL_REQUIRED
			if (token->type == CB){
				print_debug("valid }")

				GET_TOKEN
			}else{
				*sucess = 0;
				printd("}")
			}
		}else{
			*sucess = 0;
			printd("{")
		}
	}
}

//<bool_op> - < 
//<bool_op> - >
//<bool_op> - <=
//<bool_op> - >=
//<bool_op> - ==
//<bool_op> - !=
void rule_bool_op(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tokenListPtr tokenList){

	if (token->type == LT){
		print_debug("valid <")
	}else if(token->type == LTEQ){
		print_debug("valid <=")
	}else if(token->type == GT){
		print_debug("valid >")
	}else if(token->type == GTEQ){
		print_debug("valid >=")
	}else if(token->type == EQ){
		print_debug("valid ==")
	}else if(token->type == NEQ){
		print_debug("valid !=")
	}else{
		*sucess = 0;
		printd("operator rovnosti")
		return;
	}
	tokenAppend(tokenList, token, STab, NULL);
	GET_TOKEN
}

//<for> - for <var_def_cycle>; <expr_bool>; <asg_opt> { <body> }
//<var_def_cycle> - ID <var_def>
//<var_def_cycle> - epsilon
//<asg_opt> - ID = <expr>
//<expr_opt> - epsilon
void rule_for(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	do{
		STCreateFrame(STab, false);
		GET_TOKEN
		EOL_FORBID
		if (token->type == ID){
			print_debug("valid ID")
			saveToken(token);
				
			GET_TOKEN
			EOL_FORBID
			if (token->type == DEF){
				print_debug("valid :=")

				rule_var_def(PARAMS);
				if (!*sucess) break;
				EOL_FORBID
			}else{
				*sucess = 0;
				printd(" :=")
				if (!*sucess) break;
			}
		}
		if (token->type == SEM){
			print_debug("valid ;")
		}else{
			*sucess = 0;
			printd(";")
			if (!*sucess) break;
		}

		GET_TOKEN
		EOL_FORBID
		tokenListPtr tokenList = malloc(sizeof(struct tokenList));
		if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
		
		int numBool = 0;	
		rule_expr_bool(PARAMS, tokenList, &numBool);
		if (!*sucess || numBool != 1) {
			tokenListDispose(tokenList);
			free(tokenList);
			*sucess = 0;
			break;
		}
		
		if (precedence(tokenList, STab, false) == NULL){
			*sucess=0;
			tokenListDispose(tokenList);
			free(tokenList);
			printd("chyba precedence")
			break;
		}
		tokenListDispose(tokenList);
		free(tokenList);
			
		EOL_FORBID
		if (token->type == SEM){
			print_debug("valid ;")
		}else{
			*sucess = 0;
			printd(";")
			if (!*sucess) break;
		}


		GET_TOKEN
		if (token->type == ID){
			EOL_FORBID
			print_debug("valid ID")
			STVarLookUp(STab, token->attr);
			saveToken(token);
			GET_TOKEN

			if (token->type == ASG){
				print_debug("valid =")
			}else{
				*sucess = 0;
				printd("=")
				if (!*sucess) break;
			}

			GET_TOKEN
			EOL_FORBID
			tokenListPtr tokenList = malloc(sizeof(struct tokenList));
			if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);		
			rule_expr(PARAMS, tokenList);
			if (*sucess){
				if (precedence(tokenList, STab, false) == NULL){
					printd("chyba precedence");
				}else{

				//nastavy ako aktivnu premennu, tu ktoru chce skontrolovat
				STVarLookUp(STab, token->savedToken->attr);
				//skontroluje typ pomocou vlozenia
				STVarSetType(STab, tokenListGetFirstType(tokenList));
				}
			}
			tokenListDispose(tokenList);
			free(tokenList);	 
		}
	}while(0);

	CHECK_POINT(type,OB)
	
	STCreateFrame(STab,false);

	EOL_FORBID
	if (token->type == OB){
		print_debug("valid {")
		
		GET_TOKEN
		EOL_REQUIRED
		rule_body(PARAMS);
		if (!*sucess) return;

		EOL_REQUIRED
	}else{
		*sucess = 0;
		printd("{")
		if (!*sucess) return;
	}

	if (token->type == CB){
		print_debug("valid }")

		GET_TOKEN
	}else{
		*sucess = 0;
		printd("}")
		if (!*sucess) return;
	}
	STDeleteFrame(STab);
	STDeleteFrame(STab);
}


//<params> - id <type> <type_n>
//<params> - epsilon
//
//<type_n> - epsilon
void rule_params(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == ID){
		print_debug("valid ID")
		STFuncInsertParamId(STab, token->attr);		

		GET_TOKEN
		EOL_FORBID
		rule_type(PARAMS);
		if (!*sucess) return;


		EOL_FORBID
		rule_params_n(PARAMS);
		if (!sucess) return;
	}
}

//<params_n> - , id <type> <type_n>
//<params_n> - epsilon
//
//<type_n> - epsilon
void rule_params_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid , ")

		if (token->type == ID){
			print_debug("valid ID")
			STFuncInsertParamId(STab, token->attr);		
			
		}else{
			*sucess = 0;
			printd("ID")
			return;
		}

		GET_TOKEN
		EOL_FORBID
		rule_type(PARAMS);
		if (!*sucess) return;

		EOL_FORBID
		rule_params_n(PARAMS);
		if (!sucess) return;
	}
}

//<type> - int
//<type> - float64
//<type> - string
void rule_type(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == KW_INT){
		print_debug("valid KW_INT")
		STFuncInsertParamType(STab, INT_T);
	}else if (token->type == KW_FLOAT64){
		print_debug("valid KW_FLOAT64")
		STFuncInsertParamType(STab,FLOAT64_T);
	}else if (token->type == KW_STRING){
		print_debug("valid KW_STRING")
		STFuncInsertParamType(STab, STRING_T);
	}else{
		*sucess = 0;
		printd("type")
		return;
	}

	GET_TOKEN
}

//<type> - int
//<type> - float64
//<type> - string
void rule_type_r(tToken *token, tKWPtr keyWords, bool* sucess, tRetListPtr ret){
	if (token->type == KW_INT){
		print_debug("valid KW_INT")
		retListInsert(ret, INT_T);
	}else if (token->type == KW_FLOAT64){
		print_debug("valid KW_FLOAT64")
		retListInsert(ret, FLOAT64_T);
	}else if (token->type == KW_STRING){
		print_debug("valid KW_STRING")
		retListInsert(ret, STRING_T);
	}else{
		*sucess = 0;
		printd("type")
		return;
	}

	GET_TOKEN
}

//<type_n> - , <type> <type_n>
//
//<type_n> - epsilon 
void rule_type_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tRetListPtr ret){
	if (token->type == COM){
		print_debug("valid , ")

		GET_TOKEN
		EOL_FORBID
		rule_type_r(token, keyWords, sucess, ret);
		if (!*sucess) return;
		
		EOL_FORBID
		rule_type_n(PARAMS, ret);
		if (!*sucess) return;
	}
}


//<return_types> - (<type> <type_n>)
//<return_types> - epsilon
//
//<type_n> - epsilon
void rule_return_type(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == OBR){
		print_debug("valid (")
		tRetListPtr ret = malloc(sizeof(struct RetList));
		if (!ret) setError(INTERNAL_ERROR); else retListInit(ret);

		GET_TOKEN
		//EOL_OPRIONAL - nie som si isty
		rule_type_r(token, keyWords, sucess, ret);
		if (!*sucess) {
			retListDispose(ret);
			free(ret);
			return;
		}
	
		EOL_FORBID
		rule_type_n(PARAMS, ret);
		if (!*sucess) {
			retListDispose(ret);
			free(ret);
			return;
		}
		
		if (token->type == CBR){
			print_debug("valid )")
		}else{
			retListDispose(ret);
			free(ret);
			*sucess = 0;
			printd(")")
			return;
		}
		
		STFuncInsertRet(STab, ret);
		retListDispose(ret);
		free(ret);

		GET_TOKEN
		EOL_FORBID
	}else{
		STFuncInsertRet(STab, NULL);
	}	
}

