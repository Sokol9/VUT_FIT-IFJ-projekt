#include <stdio.h>
#include "project.h"
#include "syntax.h"
#include "error.h"
#include "symtable.h"
#include "expr.h"
#include "inst.h"

// <prog> - package main <func_def> <func_n> EOF
//
// <func_n> - epsilon
void rule_prog(tToken *token, tSymTablePtr STab, tKWPtr keyWords,  bool* success){
	/*************PACKAGE MAIN*****************/
	do{
		GET_TOKEN
		if (token->type == KW_PACKAGE){
			print_debug("valid KW_PACKAGE")
		}else{
			*success = 0;
			printd("KW_PACKAGE")	
			break;
		}

		START();

		if (*success){
			GET_TOKEN
			EOL_FORBID			
			if (token->type == ID && !strcmp(token->attr, "main" )){
				print_debug("valid ID-main")
				GET_TOKEN
				EOL_REQUIRED
			}else{
				*success = 0;
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
		
		//printf("\n===================================================\n\n ANALIZATION FINISH %s\n\n", 
//				*success ? "SUCESSFULL" : "WITH FAILS");
	}
	/****************END OF EOF*****************/
}


// <func_def> - func id (<params>) <return_types> {<body>}
//
// <params> - epsilon
// <return_types> - epsilon
void rule_func_def(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
	CHECK_POINT(type,KW_FUNC)	
	/*****************FUNC**********************/
	do{
		if (!*success) break;
	
		if (token->type == KW_FUNC){
			print_debug("valid KW_FUNC") 
		}else{ 
			*success = 0;
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
			*success = 0;
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
			*success = 0;
			printd("(")
			break;
		}
	/*************END OF BRACKETS***************/
	
	/*****************<PARAMS>******************/
		GET_TOKEN
		//EOL_OPTIONAL -- nie som si isty
		rule_params(PARAMS);
		if (!*success) break;
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
			*success = 0;
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
	if (!getError() && STFuncGetName(STab) != NULL){
		 DEFFUNC();
		if (!strcmp("main", STFuncGetName(STab))) INIT_MAIN();
	}
	/***************BRACKETS********************/
	if (token->type == OB){
		print_debug("valid {")
    }else{
        *success = 0;
		printd("{")
		return;
    }
	/*************END OF BRACKETS***************/

	/*****************<BODY>********************/
	GET_TOKEN
	EOL_REQUIRED
	rule_body(PARAMS);
	if (!*success) return;
	/*************END OF <BODY>*****************/

	/***************BRACKETS********************/
	if (token->type == CB){
		print_debug("valid }")
		EOL_REQUIRED


		if(!STGetFuncReturn(STab)){
			printd("func don't include rutern or return is invalid")
			setError(SEM_FUNC_ERROR);
		}	

		
		if(STFuncGetName(STab) != NULL){
			if (!strcmp("main",STFuncGetName(STab))) EXIT();		
			else RETURN();
		}
		STDeleteFrame(STab);
	}else{
		*success = 0;
		printd("}")
		return;
	}
	/*************END OF BRACKETS***************/	

	GET_TOKEN
}

//<func_n> - <func_def> <func_n>
//
//<func_n> - epsilon
void rule_func_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){

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
void rule_body(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
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
void rule_stat(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
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
			

			//===kontrola navratovych hodnot===
			//ulozenie aktivnej funkcie
			tGRPtr actFunc = STGetActiveFunc(STab);
			//activ func == call func
			STFuncInsert(STab, token->savedToken->attr, false);
			//kontrola navratovych hodnot
			tokenRetListCompare(NULL, STab);
			//nastavenie p;vodnej activ func
			STSetActiveFunc(STab, actFunc);


			GET_TOKEN
			rule_func_call(PARAMS, RULE_STAT, NULL);
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
			*success = 0;
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

			rule_expr(PARAMS, tokenListTmp, NULL);
			if (*success) {
				//precedencna analiza spracuje zoznam tokenov a vysldedny prida
				//do zoznamu urceneho na porovnanie
				if (precedence(tokenListTmp, STab, true) != NULL){
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
				tokenListDispose(tokenListRet);
				free(tokenListRet);
				return;
			}	
			
			
			rule_expr_n(PARAMS, tokenListRet);
			if (!*success){
				
				tokenListDispose(tokenListRet);
				free(tokenListRet);
				return;
			}	
			//kontrola ret of func
			//predpoklada sa, ze aktivna funkcia je ta v kotej definicii sa nachadzeme
			tokenRetListCompare(tokenListRet, STab);
			if(!getError()) funcReturnHandler(tokenListRet);
			
			tokenListDispose(tokenListRet);
			free(tokenListRet);

			STSetFuncReturn(STab);

		}
	/*************END OF RETURN******************/
	}else if(token->type != OBR){
		//follow(<stat>) == OB (})i
		//valid epsilon
		*success = 0;
		printd("}")
		GET_TOKEN
	}
}

//<func_call> - (<params_actual>)
//
//<params_actual> - epsilon
void rule_func_call(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success, callAs call, tokenListPtr tokenListL){
	
	//ulozenie aktivnej funkcie
	tGRPtr actFunc = STGetActiveFunc(STab);
	
	CREATEFRAME();	

	if (token->type != CBR){
		rule_term(token, STab, success, call, tokenListL);
		if (!*success) return;

		GET_TOKEN
		EOL_FORBID
		rule_term_n(PARAMS, call, tokenListL);
		if (!*success) return;
		
	}
	if (token->type == CBR){
		print_debug("valid )")

		STFuncParamEnd(STab);
		funcCallHandler(STab, tokenListL);
	}else{
		*success = 0;
		printd(")")
		if (!*success) return;
	}
	GET_TOKEN
		
	//nastavenie p;vodnej activ func
	STSetActiveFunc(STab, actFunc);
}

//<term> - id 
//<term> - INT_L
//<term> - FLOAT_L
//<term> - STRING_L
void rule_term(tToken *token, tSymTablePtr STab,  bool* success, callAs call, tokenListPtr tokenListL){
	if (token->type == ID){
		print_debug("valid ID")
		
		if (STVarLookUp(STab, token->attr)){
			if (call == RULE_ASG || call == RULE_STAT) STFuncInsertParamType(STab, STVarGetType(STab));
		}
	}else if (token->type == INT_L){
		print_debug("valid INT_L")
		if (call == RULE_ASG || call == RULE_STAT) STFuncInsertParamType(STab, INT_T);
	}else if (token->type == FLOAT_L){
		print_debug("valid FLOAT_L")
		if (call == RULE_ASG || call == RULE_STAT) STFuncInsertParamType(STab, FLOAT64_T);
	}else if (token->type == STRING_L){
		print_debug("valid STRING_L")
		if (call == RULE_ASG || call == RULE_STAT) STFuncInsertParamType(STab, STRING_T);
	}else{
		*success = 0;
		printd("ID or some value")
	}
	if (call == RULE_ASG || call == RULE_STAT){
		//printf("***I call token param hendler\n");
		tokenParamHandler(STab, token, tokenListL);
	}
}

//<term_n> - , <term> <term_n>
//
//<term_n> - epsilon
void rule_term_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success, callAs call, tokenListPtr tokenListL){
	if (token->type == COM){
		print_debug("valid ,")

		GET_TOKEN
		rule_term(token, STab, success, call, tokenListL);
		if (!*success) return;

		GET_TOKEN
		EOL_FORBID
		rule_term_n(PARAMS, call, tokenListL);
		if (!*success) return;
	}
}

//<var_def> -  := <expr>
void rule_var_def(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
	// := uz skontrolovan=a
	
	STVarInsert(STab, token->savedToken->attr);
	DEFVAR();	

	GET_TOKEN
	//EOL_OPTIONAL - nie som si tym isty
	tokenListPtr tokenList = malloc(sizeof(struct tokenList));
	if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
	rule_expr(PARAMS, tokenList, NULL);	
	if (*success) {
		if (precedence(tokenList, STab, true) != NULL){
			STVarLookUp(STab, token->savedToken->attr);
			STVarSetType(STab, tokenListGetFirstType(tokenList));
		}
	
		tokenListPtr tokenListL = malloc(sizeof(struct tokenList));
		if (tokenListL) tokenListInit(tokenListL); else setError(INTERNAL_ERROR);
		tokenAppend(tokenListL,token->savedToken,STab, NULL);

		tokenListAssign(tokenListL, tokenList);
		
		tokenListDispose(tokenListL);	
		free(tokenListL);
	}	
	tokenListDispose(tokenList);	
	free(tokenList);
}

//<expr> - <expr> <op> <expr>/<expr> - ( <expr> ) <expr_continue>
//<expr> - ( <expr> )
//<expr> - <term>
void rule_expr(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success, tokenListPtr tokenList,  tokenListPtr tokenList2){
	if (token->type == OBR){
		print_debug("valid (")
		tokenAppend(tokenList, token, STab, NULL);
		tokenAppend(tokenList2, token, STab, NULL);


		GET_TOKEN
		rule_expr(PARAMS, tokenList, tokenList2);
		
		if (!*success) return;
		
		if (token->type == CBR){
			print_debug("valid )")
			tokenAppend(tokenList, token, STab, NULL);
			tokenAppend(tokenList2, token, STab, NULL);
			
			EOL_FORBID

		}else{
			*success = 0;
			printd(")")
			if (!*success) return;
			
		}
		GET_TOKEN
	}else{ 
		rule_term(token, STab, success, RULE_EXPR, NULL);
		if (!*success) return; else {
			tokenAppend(tokenList, token, STab, NULL);
			tokenAppend(tokenList2, token, STab, NULL);
		}
		GET_TOKEN
	}


	if (token->type == SUB || token->type == ADD || token->type == MULT ||\
			 token->type == DIV){
		EOL_FORBID
		rule_op(token, success);
		if (!*success) return; else {
			tokenAppend(tokenList, token, STab, NULL);
			tokenAppend(tokenList2, token, STab, NULL);
		}
		GET_TOKEN
		rule_expr(PARAMS, tokenList, tokenList2);
		if (!*success) return;
	}
}

//<op> - +
//<op> - - 
//<op> - *
//<op> - /
void rule_op(tToken *token, bool* success){
	if (token->type == SUB){
		print_debug("valid -")
	}else if (token->type == ADD){
		print_debug("valid +")
	}else if (token->type == MULT){
		print_debug("valid *")
	}else if (token->type == DIV){
		print_debug("valid /")
	}else{
		*success = 0;
		printd("operand")
	}
}
	
//<var asg> - <id_n> = <values>
void rule_var_asg(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
	tokenListPtr tokenListL = malloc(sizeof(struct tokenList));
	if (!tokenListL) setError(INTERNAL_ERROR); else tokenListInit(tokenListL);
	tokenAppend(tokenListL, token->savedToken, STab, NULL);	
		

	rule_id_n(PARAMS, tokenListL);
	if (!*success) {
		tokenListDispose(tokenListL);
		free(tokenListL);
		return;
	}

	if (token->type == ASG){
	print_debug("valid =")
	}else{
		*success = 0;
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
void rule_values(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success, tokenListPtr tokenListL){
	if (token->type == OBR || token->type ==INT_L || token->type == FLOAT_L ||\
			token->type == STRING_L){
		
		tokenListPtr tokenListR = malloc(sizeof(struct tokenList));
		if (!tokenListR) setError(INTERNAL_ERROR); else tokenListInit(tokenListR);

		tokenListPtr tokenListTmp = malloc(sizeof(struct tokenList));
		if (tokenListTmp) tokenListInit(tokenListTmp); else setError(INTERNAL_ERROR);	
		rule_expr(PARAMS, tokenListTmp, NULL);
		if (!*success) {
			tokenListDispose(tokenListR);
			tokenListDispose(tokenListTmp);
			free(tokenListR);
			free(tokenListTmp);
			return;
		}
		if (precedence(tokenListTmp, STab, true) != NULL){	
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
		if (!*success){
			tokenListDispose(tokenListR);
			free(tokenListR);
			return;
		}
		// kontrola L a R strany
		tokenListAssign(tokenListL, tokenListR);

		tokenListDispose(tokenListR);
		free(tokenListR);


	}else if (token->type == ID){
		print_debug("valid ID")
		saveToken(token);
			
		GET_TOKEN
		if (token->type == OBR){
			EOL_FORBID
			print_debug("valid (")
			//===kontrola navratovych hodnot===
			//ulozenie aktivnej funkcie
			tGRPtr actFunc = STGetActiveFunc(STab);
			//activ func == call func
			STFuncInsert(STab, token->savedToken->attr, false);
			//kontrola navratovych hodnot
			tokenRetListCompare(tokenListL, STab);
			
			

			GET_TOKEN
			rule_func_call(PARAMS, RULE_ASG, tokenListL);

			//nastavenie p;vodnej activ func
			STSetActiveFunc(STab, actFunc);
			if (!*success) return;

		}else if(token->type == ADD || token->type == SUB || token->type == MULT || \
				token->type == DIV){
			EOL_FORBID
			tokenListPtr tokenListTmp = malloc(sizeof(struct tokenList));
			if (tokenListTmp) tokenListInit(tokenListTmp); else setError(INTERNAL_ERROR);
			tokenAppend(tokenListTmp, token->savedToken, STab, NULL);
				
			rule_op(token, success);
			if (!*success) {
				tokenListDispose(tokenListTmp);
				free(tokenListTmp);
				return;
			}
			tokenAppend(tokenListTmp, token, STab, NULL);

			GET_TOKEN
			rule_expr(PARAMS, tokenListTmp, NULL);
			if (!*success) {
				tokenListDispose(tokenListTmp);
				free(tokenListTmp);
				return;
			}
			tokenListPtr tokenListR = malloc(sizeof(struct tokenList));
			if (!tokenListR) setError(INTERNAL_ERROR); else tokenListInit(tokenListR);
			
			if (precedence(tokenListTmp, STab, true) != NULL){
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
			if (!*success){
				tokenListDispose(tokenListR);
				free(tokenListR);
				return;
			}
			//porovnanie L a R strany
			tokenListAssign(tokenListL, tokenListR);

			tokenListDispose(tokenListR);
			free(tokenListR);	
		}else{
			
			tokenListPtr tokenListR = malloc(sizeof(struct tokenList));
			if (!tokenListR) setError(INTERNAL_ERROR); else tokenListInit(tokenListR);
			tokenAppend(tokenListR, token->savedToken, STab, NULL);

			rule_expr_n(PARAMS, tokenListR);	
			if (!*success){
				tokenListDispose(tokenListR);
				free(tokenListR);
				return;
			}
			//porovnanie L a R strany
			tokenListAssign(tokenListL, tokenListR);

			tokenListDispose(tokenListR);
			free(tokenListR);	
		}

	}else{
		*success = 0;
		printd("some value")
	}
}

//<expr_n> - , <expr> <expr_n>
//<expr_n> - epsilon
void rule_expr_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success, tokenListPtr tokenListR){
	if (token->type == COM){
		print_debug("valid ,")
		EOL_FORBID

		GET_TOKEN
		tokenListPtr tokenListTmp = malloc(sizeof(struct tokenList));
		if (tokenListTmp) tokenListInit(tokenListTmp); else setError(INTERNAL_ERROR);
		rule_expr(PARAMS, tokenListTmp, NULL);
		if (*success) {
			if (precedence(tokenListTmp, STab, true) != NULL){
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
		if (!*success) return;
	}
}

//<id_n> - , id <id_n> 
//<id_n> - , _ <id_n>
//<id_n> - epsilon
void rule_id_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success, tokenListPtr tokenListL){
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
			if (!*success) return;
		}else{
			*success = 0;
			printd("ID")
			if (!*success) return;
		}
	}
}

// <if> - if <expr_bool> { <body> } <else>
//
// // <body> - epsilon
// <else> - epsilon
void rule_if(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
	tokenListPtr tokenList = malloc(sizeof(struct tokenList));
	if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
		
	STCreateFrame(STab, false);
	do{
		GET_TOKEN
		EOL_FORBID
		
		int numBool = 0;
		rule_expr_bool(PARAMS, tokenList, &numBool);
		if (!*success) break;
		
		if (numBool != 1) setError(SEM_TYPE_ERROR); 

		if (precedence(tokenList, STab, true) == NULL){
			*success=0;
			printd("chyba precedence");
			break;
		}
		handleStartIf(STab, tokenList);
	}while(0);
	
	tokenListDispose(tokenList);
	free(tokenList);

	CHECK_POINT(type,OB)

	if (token->type == OB){
		print_debug("valid {")
		GET_TOKEN
		EOL_REQUIRED
		rule_body(PARAMS);
		if (!*success) return;

		EOL_REQUIRED
		if (token->type == CB){
			print_debug("valid }")

			GET_TOKEN
		}else{
			*success = 0;
			printd("}")
			if (!*success) return;
		}
	}else{
		*success = 0;
		printd("{")
		if (!*success) return;
	}
	
	handleEndIf(STab, token);

	STDeleteFrame(STab);
	rule_else(PARAMS);
	
}

// <expr_bool> - <expr> s povolenim bool_op
void rule_expr_bool(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success, tokenListPtr tokenList, int* numBool){
	if (token->type == OBR){
		print_debug("valid (")
		tokenAppend(tokenList, token, STab, NULL);

		GET_TOKEN
		rule_expr_bool(PARAMS, tokenList, numBool);
		if (!*success) return;
		
		if (token->type == CBR){
			print_debug("valid )")
			tokenAppend(tokenList, token, STab, NULL);
			
			EOL_FORBID

		}else{
			*success = 0;
			printd(")")
			if (!*success) return;
			
		}
		GET_TOKEN
	}else{
		rule_term(token, STab, success, RULE_EXPR, NULL);
		if (!*success) return; else tokenAppend(tokenList, token, STab, NULL);
		
		GET_TOKEN
	}

	if (token->type == SUB || token->type == ADD || token->type == MULT ||\
			 token->type == DIV){
		EOL_FORBID
		rule_op(token, success);
		if (!*success) return; else tokenAppend(tokenList, token, STab, NULL);

		GET_TOKEN
		rule_expr_bool(PARAMS, tokenList, numBool);
		if (!*success) return;
	}else if(token->type == LT || token->type == LTEQ || token->type == GT || token->type == GTEQ ||\
			 token->type == EQ || token->type == NEQ){
		(*numBool)++;
		EOL_FORBID
		rule_bool_op(PARAMS, tokenList);

		rule_expr_bool(PARAMS, tokenList, numBool);
		if (!*success) return;
	}
}



//<else> - else { <body> }
//<else> - epsilon
//
//<body> - epsiolon
 void rule_else(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
	if (token->type == KW_ELSE)	{
		print_debug("valid KW_ELSE")
		EOL_FORBID

		GET_TOKEN
		EOL_FORBID
		if ( token->type == OB){
			print_debug("valid {")
			STCreateFrame(STab, false);
			GET_TOKEN
			EOL_REQUIRED
			rule_body(PARAMS);
			if (!*success) return;

			EOL_REQUIRED
			if (token->type == CB){
				print_debug("valid }")
				LABEL(STGetFrameNumber(STab), "");


				STDeleteFrame(STab);
			
				GET_TOKEN
			}else{
				*success = 0;
				printd("}")
			}
		}else{
			*success = 0;
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
void rule_bool_op(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success, tokenListPtr tokenList){

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
		*success = 0;
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
void rule_for(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
	tokenListPtr tokenListInc;
	bool inc = false;
	do{
		STCreateFrame(STab, false);
		//JUMP

		//nastavis flag
		//vytvoris premenne
		//
		//LABEL
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
				if (!*success) break;
				EOL_FORBID
			}else{
				*success = 0;
				printd(" :=")
				if (!*success) break;
			}
		}
		if (token->type == SEM){
			print_debug("valid ;")
		}else{
			*success = 0;
			printd(";")
			if (!*success) break;
		}
		//label begin
		LABEL(STGetFrameNumber(STab), "_begin");
		GET_TOKEN
		EOL_FORBID
		tokenListPtr tokenList = malloc(sizeof(struct tokenList));
		if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
		
		int numBool = 0;	
		rule_expr_bool(PARAMS, tokenList, &numBool);
		if (!*success || numBool != 1) {
			if (*success) setError(5);

			tokenListDispose(tokenList);
			free(tokenList);
			*success = 0;
			printd("invalid count of bool operands")
			break;
		}
		
		if (precedence(tokenList, STab, true) == NULL){
			*success=0;
			tokenListDispose(tokenList);
			free(tokenList);
			printd("chyba precedence")
			break;
		}
		//jump to end
		handleStartIf(STab,tokenList);

		tokenListDispose(tokenList);
		free(tokenList);

		EOL_FORBID
		if (token->type == SEM){
			print_debug("valid ;")
		}else{
			*success = 0;
			printd(";")
			if (!*success) break;
		}


		GET_TOKEN
		if (token->type == ID){
			inc = true;

			EOL_FORBID
			print_debug("valid ID")
			STVarLookUp(STab, token->attr);
			saveToken(token);
			GET_TOKEN

			if (token->type == ASG){
				print_debug("valid =")
			}else{
				*success = 0;
				printd("=")
				if (!*success) break;
			}


			GET_TOKEN
			EOL_FORBID
			tokenListInc = malloc(sizeof(struct tokenList));
			tokenListPtr tokenListIncTmp = malloc(sizeof(struct tokenList));
			if (tokenListInc) tokenListInit(tokenListInc); else setError(INTERNAL_ERROR);		
			if (tokenListIncTmp) tokenListInit(tokenListIncTmp); else setError(INTERNAL_ERROR);		
			rule_expr(PARAMS, tokenListInc, tokenListIncTmp);
			if (!*success){
				tokenListDispose(tokenListInc);
				free(tokenListInc);	 
				tokenListDispose(tokenListIncTmp);
				free(tokenListIncTmp);	 
				break;
			}
				//bez vypisu
			if (precedence(tokenListIncTmp, STab, false) == NULL){
				tokenListDispose(tokenListIncTmp);
				free(tokenListIncTmp);
				printd("chyba precedence")
				break;
			}
			//nastavy ako aktivnu premennu, tu ktoru chce skontrolovat
			STVarLookUp(STab, token->savedToken->attr);
			//skontroluje typ pomocou vlozenia
			STVarSetType(STab, tokenListGetFirstType(tokenListIncTmp));

			tokenListDispose(tokenListIncTmp);
			free(tokenListIncTmp);	 
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
		if (!*success){
			if (inc){ 
				tokenListDispose(tokenListInc);
				free(tokenListInc);	 
			}
			return;
		}
		EOL_REQUIRED
	}else{
		*success = 0;
		printd("{")
		if (inc){ 
			tokenListDispose(tokenListInc);
			free(tokenListInc);	 
		}
		return;
	}

	if (token->type == CB){
		print_debug("valid }")

		GET_TOKEN
	}else{
		*success = 0;
		printd("}")
		if (inc){ 
			tokenListDispose(tokenListInc);
			free(tokenListInc);	 
		}
		return;
	}
	STDeleteFrame(STab);
	if (inc){ 
		if (precedence(tokenListInc, STab, true) == NULL){
			printd("chyba precedence");		
		}
		tokenListDispose(tokenListInc);
		free(tokenListInc);	 	
	}
	// ak pomocna premenna > 0 skakat
	
	//inkrementacia pomocnej premennjie
	
	// sem
	
	//jump for begin
	handleEndFor(STab);
	//label end
		

	STDeleteFrame(STab);
}


//<params> - id <type> <type_n>
//<params> - epsilon
//
//<type_n> - epsilon
void rule_params(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
	if (token->type == ID){
		print_debug("valid ID")
		STFuncInsertParamId(STab, token->attr);		

		GET_TOKEN
		EOL_FORBID
		rule_type(PARAMS);
		if (!*success) return;


		EOL_FORBID
		rule_params_n(PARAMS);
		if (!success) return;
	}
}

//<params_n> - , id <type> <type_n>
//<params_n> - epsilon
//
//<type_n> - epsilon
void rule_params_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
	if (token->type == COM){
		print_debug("valid , ")

		GET_TOKEN
		if (token->type == ID){
			print_debug("valid ID")
			STFuncInsertParamId(STab, token->attr);		
			
		}else{
			*success = 0;
			printd("ID")
			return;
		}

		GET_TOKEN
		EOL_FORBID
		rule_type(PARAMS);
		if (!*success) return;

		EOL_FORBID
		rule_params_n(PARAMS);
		if (!success) return;
	}
}

//<type> - int
//<type> - float64
//<type> - string
void rule_type(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
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
		*success = 0;
		printd("type")
		return;
	}

	GET_TOKEN
}

//<type> - int
//<type> - float64
//<type> - string
void rule_type_r(tToken *token, tKWPtr keyWords, bool* success, tRetListPtr ret){
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
		*success = 0;
		printd("type")
		return;
	}

	GET_TOKEN
}

//<type_n> - , <type> <type_n>
//
//<type_n> - epsilon 
void rule_type_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success, tRetListPtr ret){
	if (token->type == COM){
		print_debug("valid , ")

		GET_TOKEN
		EOL_FORBID
		rule_type_r(token, keyWords, success, ret);
		if (!*success) return;
		
		EOL_FORBID
		rule_type_n(PARAMS, ret);
		if (!*success) return;
	}
}


//<return_types> - (<type> <type_n>)
//<return_types> - epsilon
//
//<type_n> - epsilon
void rule_return_type(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* success){
	if (token->type == OBR){
		print_debug("valid (")

		GET_TOKEN
		if(token->type == CBR){
			print_debug("valid )")
			STFuncInsertRet(STab, NULL);
			STSetFuncReturn(STab);	
			GET_TOKEN
			EOL_FORBID
			return;
		
		}

		tRetListPtr ret = malloc(sizeof(struct RetList));
		if (!ret) setError(INTERNAL_ERROR); else retListInit(ret);

		//EOL_OPRIONAL - nie som si isty
		rule_type_r(token, keyWords, success, ret);
		if (!*success) {
			retListDispose(ret);
			free(ret);
			return;
		}
	
		EOL_FORBID
		rule_type_n(PARAMS, ret);
		if (!*success) {
			retListDispose(ret);
			free(ret);
			return;
		}
		
		if (token->type == CBR){
			print_debug("valid )")
		}else{
			retListDispose(ret);
			free(ret);
			*success = 0;
			printd(")")
			return;
		}
		
		STFuncInsertRet(STab, ret);
		if (ret->first == NULL)	STSetFuncReturn(STab);
		retListDispose(ret);
		free(ret);

		GET_TOKEN
		EOL_FORBID
	}else{
		STFuncInsertRet(STab, NULL);
		STSetFuncReturn(STab);
	}	
}

