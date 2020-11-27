#include <stdio.h>
#include "project.h"
#include "syntax.h"
#include "error.h"

/*program neriesi 
 * pracu so symtable
 * a to je asi vsetko
*/



// <prog> - package main <func_def> <func_n> EOF
//
// <func_n> - epsilon
void rule_prog(tToken *token, tKWPtr keyWords,  bool* sucess){
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
	
	rule_func_def(token, keyWords, sucess);
	EOL_REQUIRED
 	
	rule_func_n(token, keyWords,  sucess);
	GET_TOKEN

	/*****************EOF***********************/
	if (token->type == TOKEN_EOF){
		return;
	//	print_debug("valid TOKEN_EOF")
		
	/*	printf("\n===================================================\n\n ANALIZATION FINISH %s\n\n", \
				*sucess ? "SUCESSFULL" : "WITH FAILS");*/
	}else{
		setError(SYN_ERROR);
	}
	/****************END OF EOF*****************/
}


// <func_def> - func id (<params>) <return_types> {<body>}
//
// <params> - epsilon
// <return_types> - epsilon
void rule_func_def(tToken *token, tKWPtr keyWords, bool* sucess){
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
    	    if (!strcmp(token->attr,"main")){
				print_debug("valid ID-main") 
			}else{
				print_debug("valid ID")
			}
		}else{
			*sucess = 0;
			printd("ID or main")
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
		rule_params(token, keyWords,  sucess);
		if (!*sucess) break;

		EOL_FORBID
		if (token->type == CBR){
			print_debug("valid )")
		}else{
			*sucess = 0;
		}

		GET_TOKEN
		EOL_FORBID
		rule_return_type(token, keyWords,  sucess);
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
	rule_body(token, keyWords,  sucess);
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
void rule_func_n(tToken *token, tKWPtr keyWords, bool* sucess){
	CHECK_POINT(type,KW_FUNC)
	if (token->type != TOKEN_EOF){
		rule_func_def(token, keyWords,  sucess);

		EOL_REQUIRED
		rule_func_n(token, keyWords,  sucess);
	}
}


//<body> - <stat> <body>
//
//<stat> - epsilon
//<body> - epsilon
void rule_body(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type != CB){
		rule_stat(token, keyWords,  sucess);

		CHECK_POINT(eolFlag, true)
		//podmienka, ktora odchyti chybajucu zatvorku
		if (token->type == KW_FUNC || token->type == TOKEN_EOF) return;

		EOL_REQUIRED
		rule_body(token, keyWords,  sucess);
	}
}

//<stat> - <if>
//<stat> - <for>
//<stat> - id <func_call>
//<stat> - id <var_def>
//<stat> - id <var_asg>
//<stat> - return <expr_opt> <expr_n>
//<stat> - epsilon
void rule_stat(tToken *token, tKWPtr keyWords, bool* sucess){

	if (token->type == KW_IF){
		print_debug("valid KW_IF")

		rule_if(token, keyWords,  sucess);

	}else if (token->type == KW_FOR){
		print_debug("valid KW_FOR")     
 
        rule_for(token, keyWords,  sucess);

	}else if (token->type ==  US){
		print_debug("valid _")      

		GET_TOKEN
		EOL_FORBID
		rule_var_asg(token, keyWords,  sucess);

	}else if( token->type == ID){
		print_debug("valid ID")      

		GET_TOKEN
		EOL_FORBID
		if (token->type == OBR){
			print_debug("valid (")

			GET_TOKEN
	     	rule_func_call(token, keyWords,  sucess);
          
        }else if (token->type == DEF){
			print_debug("valid :=")   
   
			rule_var_def(token, keyWords,  sucess);

		}else if (token->type == ASG || token->type ==COM){

			rule_var_asg(token, keyWords,  sucess);

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
	
			rule_expr(token, keyWords,  sucess);
			if (!*sucess) return;
		
			rule_expr_n(token, keyWords,  sucess);
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
void rule_func_call(tToken *token, tKWPtr keyWords, bool* sucess){

	if (token->type != CBR){

		rule_term(token,sucess);
		if (!*sucess) return;

		GET_TOKEN
		EOL_FORBID
		rule_term_n(token,keyWords,sucess);
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
void rule_term_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid ,")

		GET_TOKEN
		rule_term(token,sucess);
		if (!*sucess) return;

		GET_TOKEN
		EOL_FORBID
		rule_term_n(token,keyWords,sucess);
		if (!*sucess) return;
	}
}

//<var_def> -  := <expr>
void rule_var_def(tToken *token, tKWPtr keyWords, bool* sucess){
	// := uz skontrolovan=a
	GET_TOKEN
	//EOL_OPTIONAL - nie som si tym isty
	rule_expr(token,keyWords,sucess);	
	if (!*sucess) return;
}

//<expr> - <expr> <op> <expr>
//<expr> - ( <expr> )
//<expr> - <term>
void rule_expr(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == OBR){
		print_debug("valid (")

		GET_TOKEN
		rule_expr(token,keyWords,sucess);
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
		rule_expr(token,keyWords,sucess);
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
void rule_var_asg(tToken *token, tKWPtr keyWords, bool* sucess){

	rule_id_n(token,keyWords,sucess);
	if (!*sucess) return;

	if (token->type == ASG){
	print_debug("valid =")	

	}else{
		*sucess = 0;
		printd("=")
		if (!*sucess) return;
	}

	GET_TOKEN
	rule_values(token,keyWords,sucess);
	if (!*sucess) return;
}

//<values> - <expr> <expr_n>
//<values> - id <func_call>
//<values> - id <op> <expr>    --> <expr> - <expr> <op> <expr>
void rule_values(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == OBR || token->type ==INT_L || token->type == FLOAT_L ||\
			token->type == STRING_L){

		rule_expr(token,keyWords,sucess);
		if (!*sucess) return;

		rule_expr_n(token,keyWords,sucess);
		if (!*sucess) return;

	}else if (token->type == ID){
		print_debug("valid ID")

		GET_TOKEN
		EOL_FORBID
		if (token->type == OBR){
			print_debug("valid (")

			GET_TOKEN
			rule_func_call(token,keyWords,sucess);
			if (!*sucess) return;

		}else if(token->type == ADD || token->type == SUB || token->type == MULT || \
				token->type == DIV){

			rule_op(token, sucess);
			if (!*sucess) return;

			GET_TOKEN
			rule_expr(token,keyWords,sucess);
			if (!*sucess) return;
			
			rule_expr_n(token,keyWords,sucess);
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
void rule_expr_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid ,")
		EOL_FORBID

		GET_TOKEN
		rule_expr(token,keyWords,sucess);
		if (!*sucess) return;

        rule_expr_n(token,keyWords,sucess);
		if (!*sucess) return;
	}
}

//<id_n> - , id <id_n> 
//<id_n> - , _ <id_n>
//<id_n> - epsilon
void rule_id_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid ,  ")
		EOL_FORBID

		GET_TOKEN
		EOL_FORBID //nie som si isty
		if (token->type == ID || token->type == US){
			print_debug("valid ID or _")
			GET_TOKEN

			rule_id_n(token,keyWords,sucess);
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
void rule_if(tToken *token, tKWPtr keyWords, bool* sucess){
	do{
		GET_TOKEN
		EOL_FORBID
		rule_expr(token,keyWords,sucess);
		if (!*sucess) break;

		if (token->type != OB){
			rule_expr_bool(token,keyWords,sucess);
			if (!*sucess) break;
		}
	}while(0);

	CHECK_POINT(type,OB)

	if (token->type == OB){
		print_debug("valid {")

		GET_TOKEN
		EOL_REQUIRED
		rule_body(token,keyWords,sucess);
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

	rule_else(token,keyWords,sucess);
}

// <expr_bool> - <bool_op> <expr>
// <expr_bool> - epsilon
void rule_expr_bool(tToken *token, tKWPtr keyWords, bool* sucess){
	EOL_FORBID
	rule_bool_op(token,keyWords,sucess);
	if (!*sucess) return;

	rule_expr(token,keyWords,sucess);	
}


//<else> - else { <body> }
//<else> - epsilon
//
//<body> - epsiolon
 void rule_else(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == KW_ELSE)	{
		print_debug("valid KW_ELSE")
		EOL_FORBID

		GET_TOKEN
		EOL_FORBID
		if ( token->type == OB){
			print_debug("valid {")

			GET_TOKEN
			EOL_REQUIRED
			rule_body(token,keyWords,sucess);
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
void rule_for(tToken *token, tKWPtr keyWords, bool* sucess){
	do{
		GET_TOKEN
		EOL_FORBID
		if (token->type == ID){
			print_debug("valid ID")
				
			GET_TOKEN
			EOL_FORBID
			if (token->type == DEF){
				print_debug("valid :=")

				rule_var_def(token,keyWords,sucess);
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
		rule_expr(token,keyWords,sucess);
		if (!*sucess) break;
				
		rule_expr_bool(token,keyWords,sucess);
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
			rule_expr(token,keyWords,sucess);
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
			rule_expr(token,keyWords,sucess);
			if (!*sucess) break;
		}
	}while(0);

	CHECK_POINT(type,OB)
	

	EOL_FORBID
	if (token->type == OB){
		print_debug("valid {")
		
		GET_TOKEN
		EOL_REQUIRED
		rule_body(token,keyWords,sucess);
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
void rule_params(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == ID){
		print_debug("valid ID")

		GET_TOKEN
		EOL_FORBID
		rule_type(token,keyWords,sucess);
		if (!*sucess) return;

		EOL_FORBID
		rule_params_n(token,keyWords,sucess);
		if (!sucess) return;
	}
}

//<params_n> - , id <type> <type_n>
//<params_n> - epsilon
//
//<type_n> - epsilon
void rule_params_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid , ")

		if (token->type == ID){
			print_debug("valid ID")
		}else{
			*sucess = 0;
			printd("ID")
			return;
		}

		GET_TOKEN
		EOL_FORBID
		rule_type(token,keyWords,sucess);
		if (!*sucess) return;

		EOL_FORBID
		rule_params_n(token,keyWords,sucess);
		if (!sucess) return;
	}
}

//<type> - int
//<type> - float64
//<type> - string
void rule_type(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == KW_INT){
		print_debug("valid KW_INT")
	}else if (token->type == KW_FLOAT64){
		print_debug("valid KW_FLOAT64")
	}else if (token->type == KW_STRING){
		print_debug("valid KW_STRING")
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
void rule_type_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid , ")

		GET_TOKEN
		EOL_FORBID
		rule_type(token,keyWords,sucess);
		if (!*sucess) return;
		
		EOL_FORBID
		rule_type_n(token,keyWords,sucess);
		if (!*sucess) return;
	}
}


//<return_types> - (<type> <type_n>)
//<return_types> - epsilon
//
//<type_n> - epsilon
void rule_return_type(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == OBR){
		print_debug("valid (")


		GET_TOKEN
		//EOL_OPRIONAL - nie som si isty
		rule_type(token,keyWords,sucess);
		if (!*sucess) return;
	
		EOL_FORBID
		rule_type_n(token,keyWords,sucess);
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

