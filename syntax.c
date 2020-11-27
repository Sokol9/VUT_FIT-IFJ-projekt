#include <stdio.h>
#include "project.h"
#include "syntax.h"
#include "error.h"
#include "symtable.h"

/*program neriesi 
 * pracu so symtable
 * a to je asi vsetko
*/


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

	bool sem_sucess = true;
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
			sem_sucess = STFuncInsert(STab, token->attr, true); 
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
		rule_params(PARAMS, &sem_sucess);
		if (!*sucess) break;

		EOL_FORBID
		if (token->type == CBR){
			print_debug("valid )")
		}else{
			*sucess = 0;
		}

		GET_TOKEN
		EOL_FORBID
		rule_return_type(PARAMS, &sem_sucess);
	}while(0);

	CHECK_POINT(type,OB)

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

		GET_TOKEN
		EOL_FORBID
		rule_var_asg(PARAMS);

	}else if( token->type == ID){
		print_debug("valid ID")      

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
	
			rule_expr(PARAMS);
			if (!*sucess) return;
		
			rule_expr_n(PARAMS);
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

	if (token->type != CBR){

		rule_term(token,sucess);
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
void rule_term(tToken *token,  bool* sucess){
	if (token->type == ID){
		print_debug("valid ID")
	}else if (token->type == INT_L){
		print_debug("valid INT_L")
	}else if (token->type == FLOAT_L){
		print_debug("valid FLOAT_L")
	}else if (token->type == STRING_L){
		print_debug("valid STRING_L")
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
		rule_term(token,sucess);
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
	GET_TOKEN
	//EOL_OPTIONAL - nie som si tym isty
	rule_expr(PARAMS);	
	if (!*sucess) return;
}

//<expr> - <expr> <op> <expr>
//<expr> - ( <expr> )
//<expr> - <term>
void rule_expr(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == OBR){
		print_debug("valid (")

		GET_TOKEN
		rule_expr(PARAMS);
		if (!*sucess) return;
		
		if (token->type == CBR){
			print_debug("valid )")
			EOL_FORBID

		}else{
			*sucess = 0;
			printd(")")
			if (!*sucess) return;
			
		}
	}else{ 
		rule_term(token,sucess);
		if (!*sucess) return;
		
		GET_TOKEN
	}


	if (token->type == SUB || token->type == ADD || token->type == MULT ||\
			 token->type == DIV){
		EOL_FORBID
		rule_op(token, sucess);
		if (!*sucess) return;

		GET_TOKEN
		rule_expr(PARAMS);
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

	rule_id_n(PARAMS);
	if (!*sucess) return;

	if (token->type == ASG){
	print_debug("valid =")	

	}else{
		*sucess = 0;
		printd("=")
		if (!*sucess) return;
	}

	GET_TOKEN
	rule_values(PARAMS);
	if (!*sucess) return;
}

//<values> - <expr> <expr_n>
//<values> - id <func_call>
//<values> - id <op> <expr>    --> <expr> - <expr> <op> <expr>
void rule_values(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == OBR || token->type ==INT_L || token->type == FLOAT_L ||\
			token->type == STRING_L){

		rule_expr(PARAMS);
		if (!*sucess) return;

		rule_expr_n(PARAMS);
		if (!*sucess) return;

	}else if (token->type == ID){
		print_debug("valid ID")

		GET_TOKEN
		EOL_FORBID
		if (token->type == OBR){
			print_debug("valid (")

			GET_TOKEN
			rule_func_call(PARAMS);
			if (!*sucess) return;

		}else if(token->type == ADD || token->type == SUB || token->type == MULT || \
				token->type == DIV){

			rule_op(token, sucess);
			if (!*sucess) return;

			GET_TOKEN
			rule_expr(PARAMS);
			if (!*sucess) return;
			
			rule_expr_n(PARAMS);
			if (!*sucess) return;
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
void rule_expr_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid ,")
		EOL_FORBID

		GET_TOKEN
		rule_expr(PARAMS);
		if (!*sucess) return;

        rule_expr_n(PARAMS);
		if (!*sucess) return;
	}
}

//<id_n> - , id <id_n> 
//<id_n> - , _ <id_n>
//<id_n> - epsilon
void rule_id_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid ,  ")
		EOL_FORBID

		GET_TOKEN
		EOL_FORBID //nie som si isty
		if (token->type == ID || token->type == US){
			print_debug("valid ID or _")
			GET_TOKEN

			rule_id_n(PARAMS);
			if (!*sucess) return;
		}else{
			*sucess = 0;
			printd("ID")
			if (!*sucess) return;
		}
	}
}

// <if> - if <expr> <expr_bool> { <body> } <else>
//
// <expr_bool> - epsilon
// <body> - epsilon
// <else> - epsilon
void rule_if(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	do{
		GET_TOKEN
		EOL_FORBID
		rule_expr(PARAMS);
		if (!*sucess) break;

		if (token->type != OB){
			rule_expr_bool(PARAMS);
			if (!*sucess) break;
		}
	}while(0);

	CHECK_POINT(type,OB)

	if (token->type == OB){
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
			if (!*sucess) return;
		}
	}else{
		*sucess = 0;
		printd("{")
		if (!*sucess) return;
	}

	rule_else(PARAMS);
}

// <expr_bool> - <bool_op> <expr>
// <expr_bool> - epsilon
void rule_expr_bool(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	EOL_FORBID
	rule_bool_op(token, keyWords, sucess);
	if (!*sucess) return;

	rule_expr(PARAMS);	
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
void rule_bool_op(tToken *token, tKWPtr keyWords, bool* sucess){

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
	GET_TOKEN
}

//<for> - for <var_def_cycle>; <expr_bool>; <asg_opt> { <body> }
//<var_def_cycle> - ID <var_def>
//<var_def_cycle> - epsilon
//<asg_opt> - <expr> = <expr>
//<expr_opt> - epsilon
void rule_for(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess){
	do{
		GET_TOKEN
		EOL_FORBID
		if (token->type == ID){
			print_debug("valid ID")
				
			GET_TOKEN
			EOL_FORBID
			if (token->type == DEF){
				print_debug("valid :=")

				rule_var_def(PARAMS);
				if (!*sucess) break
;
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
		rule_expr(PARAMS);
		if (!*sucess) break;
				
		rule_expr_bool(PARAMS);
		if (!*sucess) break;
			
		EOL_FORBID
		if (token->type == SEM){
			print_debug("valid ;")
		}else{
			*sucess = 0;
			printd(";")
			if (!*sucess) break;
		}


		GET_TOKEN

		if (token->type != OB){
			EOL_FORBID
			rule_expr(PARAMS);
			if (!*sucess) break;

			if (token->type == ASG){
				print_debug("valid =")
			}else{
				*sucess = 0;
				printd("=")
				if (!*sucess) break;
			}

			GET_TOKEN
			EOL_FORBID
			rule_expr(PARAMS);
			if (!*sucess) break;
		}
	}while(0);

	CHECK_POINT(type,OB)
	

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
}


//<params> - id <type> <type_n>
//<params> - epsilon
//
//<type_n> - epsilon
void rule_params(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, bool* sem_sucess){
	if (token->type == ID){
		print_debug("valid ID")

		GET_TOKEN
		EOL_FORBID
		rule_type(PARAMS, sem_sucess);
		if (!*sucess) return;


		EOL_FORBID
		rule_params_n(PARAMS, sem_sucess);
		if (!sucess) return;
	}
}

//<params_n> - , id <type> <type_n>
//<params_n> - epsilon
//
//<type_n> - epsilon
void rule_params_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, bool* sem_sucess){
	if (token->type == COM){
		print_debug("valid , ")

		if (token->type == ID){
			print_debug("valid ID")
			if (*sem_sucess) STVarInsert(STab, token->attr);				
			
		}else{
			*sucess = 0;
			printd("ID")
			return;
		}

		GET_TOKEN
		EOL_FORBID
		rule_type(PARAMS, sem_sucess);
		if (!*sucess) return;

		EOL_FORBID
		rule_params_n(PARAMS, sem_sucess);
		if (!sucess) return;
	}
}

//<type> - int
//<type> - float64
//<type> - string
void rule_type(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, bool* sem_sucess){
	if (token->type == KW_INT){
		print_debug("valid KW_INT")
		if (*sem_sucess) STVarSetType(STab, INT_T);
	}else if (token->type == KW_FLOAT64){
		print_debug("valid KW_FLOAT64")
		if (*sem_sucess) STVarSetType(STab, FLOAT64_T);
	}else if (token->type == KW_STRING){
		print_debug("valid KW_STRING")
		if (*sem_sucess) STVarSetType(STab, STRING_T);
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
void rule_type_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, bool* sem_sucess){
	if (token->type == COM){
		print_debug("valid , ")

		GET_TOKEN
		EOL_FORBID
		rule_type(PARAMS, sem_sucess);
		if (!*sucess) return;
		
		EOL_FORBID
		rule_type_n(PARAMS, sem_sucess);
		if (!*sucess) return;
	}
}


//<return_types> - (<type> <type_n>)
//<return_types> - epsilon
//
//<type_n> - epsilon
void rule_return_type(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, bool* sem_sucess){
	if (token->type == OBR){
		print_debug("valid (")


		GET_TOKEN
		//EOL_OPRIONAL - nie som si isty
		rule_type(PARAMS, sem_sucess);
		if (!*sucess) return;
	
		EOL_FORBID
		rule_type_n(PARAMS, sem_sucess);
		if (!*sucess) return;
		
		if (token->type == CBR){
			print_debug("valid )")
		}else{
			*sucess = 0;
			printd(")")
			return;
		}

		GET_TOKEN
		EOL_FORBID
	}
	
}

