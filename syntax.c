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
	
	rule_func_def(PARAMS);
	EOL_REQUIRED
 	
	rule_func_n(PARAMS);
	GET_TOKEN

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

	do{
		if (!*sucess) break;
	
		if (token->type == KW_FUNC){
			print_debug("valid KW_FUNC") 
		}else{ 
			*sucess = 0;
			printd("KW_FUNC")
			break;
		}
	
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
			
		GET_TOKEN
		EOL_FORBID
		if (token->type == OBR){
			print_debug("valid (")
		}else{
			*sucess = 0;
			printd("(")
			break;
		}
	
		GET_TOKEN
		//EOL_OPTIONAL -- nie som si isty
		rule_params(PARAMS);
		if (!*sucess) break;
		STFuncParamEnd(STab);

		EOL_FORBID
		if (token->type == CBR){
			print_debug("valid )")
		}else{
			*sucess = 0;
		}

		GET_TOKEN
		EOL_FORBID
		rule_return_type(PARAMS);
	}while(0);
	
	
	CHECK_POINT(type,OB)
	STCreateFrame(STab, true);

	if (token->type == OB){
		print_debug("valid {")
    }else{
        *sucess = 0;
		printd("{")
		return;
    }

	GET_TOKEN
	EOL_REQUIRED
	rule_body(PARAMS);
	if (!*sucess) return;
	
	if (token->type == CB){
		print_debug("valid }")
		EOL_REQUIRED
		STDeleteFrame(STab);
	}else{
		*sucess = 0;
		printd("}")
		return;
	}

	GET_TOKEN
}

//<func_n> - <func_def> <func_n>
//
//<func_n> - epsilon
void rule_func_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	CHECK_POINT(type,KW_FUNC)
	if (token->type != TOKEN_EOF){
		rule_func_def(PARAMS);

		EOL_REQUIRED
		rule_func_n(PARAMS);
	}
}


//<body> - <stat> <body>
//
//<stat> - epsilon
//<body> - epsilon
void rule_body(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type != CB){
		rule_stat(PARAMS);

		CHECK_POINT(eolFlag, true)
		//podmienka, ktora odchyti chybajucu zatvorku
		if (token->type == KW_FUNC || token->type == TOKEN_EOF) return;

		EOL_REQUIRED
		rule_body(PARAMS);
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
		print_debug("valid KW_IF")

		rule_if(PARAMS);

	}else if (token->type == KW_FOR){
		print_debug("valid KW_FOR")     
 
        rule_for(PARAMS);

	}else if (token->type ==  US){
		print_debug("valid _")      
		saveToken(token);

		GET_TOKEN
		EOL_FORBID
		rule_var_asg(PARAMS);

	}else if( token->type == ID){
		print_debug("valid ID")      
		saveToken(token);		

		GET_TOKEN
		EOL_FORBID
		if (token->type == OBR){
			print_debug("valid (")

			GET_TOKEN
	     	rule_func_call(PARAMS);
          
        }else if (token->type == DEF){
			print_debug("valid :=")   
   
			rule_var_def(PARAMS);

		}else if (token->type == ASG || token->type ==COM){

			rule_var_asg(PARAMS);

		}else{
			*sucess = 0;
			printd("need valid operand")	
		}

	}else if(token->type == KW_RETURN){
		print_debug("valid KW_RETURN")
		//>>>expr_opt???? v pripade eps potrebujem striktne vratit hodnotu EOL, 
		//resp, pristupit k nej ako k hodnote, pre potrebu kontroli

		//forbid EOL strazi semanticky analiyator, podla toho, kolko exp tu ocakava
		GET_TOKEN
		if (!token->eolFlag){
			tRetListPtr retList = malloc(sizeof(struct RetList))	;
			if (retList) retListInit(retList); else setError(INTERNAL_ERROR);

			tokenListPtr tokenList = malloc(sizeof(struct tokenList));
			if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
			rule_expr(PARAMS, tokenList);
			if (*sucess) {
				if (precedence(tokenList, STab, false) != NULL){
					retListInsert(retList, tokenListGetFirstType(tokenList));
				tokenListDispose(tokenList);
				free(tokenList);	
				}
			}else{
				tokenListDispose(tokenList);
				free(tokenList);
				retListDispose(retList);
				free(retList);
				return;
			}	
			
			
			rule_expr_n(PARAMS, retList);
			if (!*sucess){
				retListDispose(retList);
				free(retList);
				return;
			}	
			//kontrola ret	
			STFuncInsertRet(STab, retList);
			retListDispose(retList);
			free(retList);
			
		}

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
	STFuncInsert(STab, token->oldAttr, false);	

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
	
	STVarInsert(STab, token->oldAttr);
	GET_TOKEN
	//EOL_OPTIONAL - nie som si tym isty
	tokenListPtr tokenList = malloc(sizeof(struct tokenList));
	if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
	rule_expr(PARAMS, tokenList);	
	if (*sucess) {
		if (precedence(tokenList, STab, false) != NULL){
			STVarLookUp(STab, token->oldAttr);
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
		tokenAppend(tokenList, token);

		GET_TOKEN
		rule_expr(PARAMS, tokenList);
		
		if (!*sucess) return;
		
		if (token->type == CBR){
			print_debug("valid )")
			tokenAppend(tokenList, token);
			
			EOL_FORBID

		}else{
			*sucess = 0;
			printd(")")
			if (!*sucess) return;
			
		}
		GET_TOKEN
	}else{ 
		rule_term(token, STab, sucess, RULE_EXPR);
		if (!*sucess) return; else tokenAppend(tokenList, token);
		
		GET_TOKEN
	}


	if (token->type == SUB || token->type == ADD || token->type == MULT ||\
			 token->type == DIV){
		EOL_FORBID
		rule_op(token, sucess);
		if (!*sucess) return; else tokenAppend(tokenList, token);

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
	tRetListPtr retL = malloc(sizeof(struct RetList));
	if (!retL) setError(INTERNAL_ERROR); else retListInit(retL);
	if (token->oldType == ID){
		STVarLookUp(STab, token->oldAttr);
		retListInsert(retL,STVarGetType(STab));	
	}else{
		retListInsert(retL,UNKNOWN_T);
	}	
		

	rule_id_n(PARAMS, retL);
	if (!*sucess) {
		retListDispose(retL);
		free(retL);
		return;
	}

	if (token->type == ASG){
	print_debug("valid =")
	}else{
		*sucess = 0;
		printd("=")
		retListDispose(retL);
		free(retL);
		return;
	}

	GET_TOKEN
	rule_values(PARAMS, retL);
	retListDispose(retL);
	free(retL);
}

//<values> - <expr> <expr_n>
//<values> - id <func_call>
//<values> - id <op> <expr>    --> <expr> - <expr> <op> <expr>
void rule_values(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tRetListPtr retL){
	if (token->type == OBR || token->type ==INT_L || token->type == FLOAT_L ||\
			token->type == STRING_L){
		
		tRetListPtr retR = malloc(sizeof(struct RetList));
		if (!retR) setError(INTERNAL_ERROR); else retListInit(retR);

		tokenListPtr tokenList = malloc(sizeof(struct tokenList));
		if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);	
		rule_expr(PARAMS, tokenList);
		if (!*sucess) {
			retListDispose(retR);
			tokenListDispose(tokenList);
			free(retR);
			free(tokenList);
			return;
		}
		if (precedence(tokenList, STab, false) != NULL){	
			retListInsert(retR, tokenListGetFirstType(tokenList));
		}
		tokenListDispose(tokenList);
		free(tokenList);

		rule_expr_n(PARAMS, retR);
		if (!*sucess){
			retListDispose(retR);
			free(retR);
			return;
		}
		//todo
		//retL vs retR check

		retListDispose(retR);
		free(retR);

	}else if (token->type == ID){
		print_debug("valid ID")
		saveToken(token);
		
		GET_TOKEN
		EOL_FORBID
		if (token->type == OBR){
			print_debug("valid (")
			STFuncInsert(STab, token->oldAttr, false);
			//kontrola navratovych hodnot
			STFuncInsertRet(STab, retL);
	
			GET_TOKEN
			rule_func_call(PARAMS);
			if (!*sucess) return;

		}else if(token->type == ADD || token->type == SUB || token->type == MULT || \
				token->type == DIV){
			tokenListPtr tokenList = malloc(sizeof(struct tokenList));
			if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
			//todo
			//tokenAppendOld
			rule_op(token, sucess);
			if (!*sucess) {
				tokenListDispose(tokenList);
				free(tokenList);
				return;
			}
			tokenAppend(tokenList, token);

			GET_TOKEN
			rule_expr(PARAMS, tokenList);
			if (!*sucess) {
				tokenListDispose(tokenList);
				free(tokenList);
				return;
			}
			tRetListPtr retR = malloc(sizeof(struct RetList));
			if (!retR) setError(INTERNAL_ERROR); else retListInit(retR);
			
			if (precedence(tokenList, STab, false) != NULL){
				 retListInsert(retR,tokenListGetFirstType(tokenList));
			}	
			tokenListDispose(tokenList);
			free(tokenList);

			
			rule_expr_n(PARAMS, retR);
			if (!*sucess){
				retListDispose(retR);
				free(retR);
				return;
			}
			//todo
			//retL vs retR check
			
			retListDispose(retR);
			free(retR);	
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
void rule_expr_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tRetListPtr retList){
	if (token->type == COM){
		print_debug("valid ,")
		EOL_FORBID

		GET_TOKEN
		tokenListPtr tokenList = malloc(sizeof(struct tokenList));
		if (tokenList) tokenListInit(tokenList); else setError(INTERNAL_ERROR);
		rule_expr(PARAMS, tokenList);
		if (*sucess) {
			if (precedence(tokenList, STab, false) != NULL){
				retListInsert(retList, tokenListGetFirstType(tokenList));
				tokenListDispose(tokenList);
				free(tokenList);
			}else{
				tokenListDispose(tokenList);
				free(tokenList);
				return;
			}	
		}

        rule_expr_n(PARAMS, retList);
		if (!*sucess) return;
	}
}

//<id_n> - , id <id_n> 
//<id_n> - , _ <id_n>
//<id_n> - epsilon
void rule_id_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tRetListPtr ret){
	if (token->type == COM){
		print_debug("valid ,  ")
		EOL_FORBID

		GET_TOKEN
		EOL_FORBID //nie som si isty
		if (token->type == ID || token->type == US){
			if (token->type == ID){
				STVarLookUp(STab, token->attr);
				retListInsert(ret, STVarGetType(STab));
			}else{
				retListInsert(ret,UNKNOWN_T);	
			}
			print_debug("valid ID or _")
			GET_TOKEN

			rule_id_n(PARAMS, ret);
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
		//todo
		//aky vyznam tu ma precedencka rovnako ako vo fore asi iba na generaciu kodu
//			STVarLookUp(STab, token-oldAttr);
//			STVarSetType(Stab, tokenListGetFirstType(tokenList));
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
		tokenAppend(tokenList, token);

		GET_TOKEN
		rule_expr_bool(PARAMS, tokenList, numBool);
		if (!*sucess) return;
		
		if (token->type == CBR){
			print_debug("valid )")
			tokenAppend(tokenList, token);
			
			EOL_FORBID

		}else{
			*sucess = 0;
			printd(")")
			if (!*sucess) return;
			
		}
		GET_TOKEN
	}else{
		rule_term(token, STab, sucess, RULE_EXPR);
		if (!*sucess) return; else tokenAppend(tokenList, token);
		
		GET_TOKEN
	}

	if (token->type == SUB || token->type == ADD || token->type == MULT ||\
			 token->type == DIV){
		EOL_FORBID
		rule_op(token, sucess);
		if (!*sucess) return; else tokenAppend(tokenList, token);

		GET_TOKEN
		rule_expr_bool(PARAMS, tokenList, numBool);
		if (!*sucess) return;
	}else if(token->type == LT || token->type == LTEQ || token->type == GT || token->type == GTEQ ||\
			 token->type == EQ || token->type == NEQ){
		(*numBool)++;
		EOL_FORBID
		rule_bool_op(token, keyWords, sucess, tokenList);

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
void rule_bool_op(tToken *token, tKWPtr keyWords, bool* sucess, tokenListPtr tokenList){

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
	tokenAppend(tokenList, token);
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
//todo
//je tu potrebna precedencna?
//podla mna je to blbost
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
			printd("chyba precedence")
			break;
//		toto je kktina tu by podla mna malo byt sucess = 0
		//	STVarLookUp(STab, token-oldAttr);
		//	STVarSetType(Stab, tokenListGetFirstType(tokenList));
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
		//todo
		//chck sem of expr
		if (token->type == ID){
			EOL_FORBID
			print_debug("valid ID")
			STVarInsert(STab, token->attr);
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
				STVarLookUp(STab, token->oldAttr);
				STVarSetType(STab, tokenListGetFirstType(tokenList));
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

