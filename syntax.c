#include <stdio.h>
#include "project.h"
#include "syntax.h"

#define DEBUG
#define getToken getToken(token,  keyWords);


#ifdef DEBUG
	#define print_debug(fsm) \
			 printf("%s, sucess: %d - line:%d - %s\n",\
			 fsm, *sucess,__LINE__,  __func__);
#else
	#define print_debug(fsm)
#endif

/*program neriesi 
 * EOL
 * zotavenie z chyb
 * pracu so symtable
 * a to je asi vsetko
*/


// <prog> - [KW_PACKAGE] [ID]-main EOL <def_func> EOL <func_n> EOF
void rule_prog(tToken *token, tKWPtr keyWords,  bool* sucess){
	/*************PACKAGE MAIN*****************/
	getToken
	if (token->type == KW_PACKAGE){
		print_debug("valid KW_PACKAGE")
	}else{
		*sucess = 0;
	}
	getToken		
	if (token->type == ID && !strcmp(token->attr, "main" )){
    	print_debug("valid ID-main")
		getToken
	}else{
		*sucess = 0;
	}
	/**********END OF PACKEGE MAIN *************/

	rule_func_n(token, keyWords,  sucess);
	getToken
	
	/*****************EOF***********************/
	if (token->type == TOKEN_EOF){
		print_debug("valid TOKEN_EOF")
		
		printf("\n===================================================\n\n ANALIZATION FINISH %s\n\n", \
				*sucess ? "SUCESSFULL" : "WITH FAILS");
	}
	/****************END OF EOF*****************/
}


// <func_def> - [KW_FUNC] [ID] (<params>) <return_types> {<body>}
//
// <params> - epsilon
// <return_types> - epsilom
void rule_func_def(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == KW_FUNC){
		print_debug("valid KW_FUNC") 
		getToken
	}else{ 
		*sucess = 0;
	}


	if (token->type == ID){
        if (!strcmp(token->attr,"main")){
			print_debug("valid ID-main") 
		}else{
			print_debug("valid ID")
		}
		getToken
	}else{
		*sucess = 0;
	}

	
	if (token->type == OBR){
		print_debug("valid (")
		getToken
	}else{
		*sucess = 0;
	}


	rule_params(token, keyWords,  sucess);


	if (token->type == CBR){
		print_debug("valid )")
		getToken
	}else{
		*sucess = 0;
	}


	rule_return_type(token, keyWords,  sucess);


	if (token->type == OB){
		print_debug("valid {")
		getToken
    }else{
        *sucess = 0;
    }

	
	rule_body(token, keyWords,  sucess);

	
	if (token->type == CB){
		print_debug("valid }")
		getToken
	}else{
		*sucess = 0;
	} 	
}

//<func_n> - <func_def> <func_n>
//
//<func_n> - epsilon
void rule_func_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type != TOKEN_EOF){
		rule_func_def(token, keyWords,  sucess);
		rule_func_n(token, keyWords,  sucess);
	}
}


//<body> - <stat>  <body>
//
//<stat> - epsilon
//<body> - epsilon
void rule_body(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type != CB){
		rule_stat(token, keyWords,  sucess);
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
		getToken
		rule_var_asg(token, keyWords,  sucess);
	}else if( token->type == ID){
		print_debug("valid ID")      
		getToken
		if (token->type == OBR){
			print_debug("valid (")
			getToken      
	     	rule_func_call(token, keyWords,  sucess);
          
        }else if (token->type == DEF){
			print_debug("valid :=")      
			rule_var_def(token, keyWords,  sucess);
		}else if (token->type == ASG || token->type ==COM){
			rule_var_asg(token, keyWords,  sucess);
		}else{
			*sucess = 0;
		}
	}else if(token->type == KW_RETURN){
		print_debug("valid KW_RETURN")
		//>>>expr_opt???? v pripade eps potrebujem striktne vratit hodnotu EOL, 
		//resp, pristupit k nej ako k hodnote, pre potrebu kontroli
		getToken
		rule_expr(token, keyWords,  sucess);
		rule_expr_n(token, keyWords,  sucess);
	}
}

//<func_call> - ( <params_actual>)
//
//<params_actual> - epsilon
void rule_func_call(tToken *token, tKWPtr keyWords, bool* sucess){

	if (token->type != CBR){
		rule_term(token,sucess);
		getToken
		rule_term_n(token,keyWords,sucess);
	}
	if (token->type == CBR){
		print_debug("valid )")
		getToken
	}else{
		*sucess = 0;
	}
	
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
	}
}

//<term_n> - , <term> <term_n>
//
//<term_n> - epsilon
void rule_term_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid ,")
		getToken
		rule_term(token,sucess);
		getToken
		rule_term_n(token,keyWords,sucess);
	}
}

//<var_def> -  := <expr>
void rule_var_def(tToken *token, tKWPtr keyWords, bool* sucess){
	// := uz skontrolovan=a
	getToken
	rule_expr(token,keyWords,sucess);	
}

//<expr> - <expr> <op> <expr>
//<expr> - ( <expr> )
//<expr> - <term>
void rule_expr(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == OBR){
		print_debug("valid (")
		getToken
		rule_expr(token,keyWords,sucess);
		if (token->type == CBR){
			print_debug("valid )")
		}else{
			*sucess = 0;
		}
	}else{ 
		rule_term(token,sucess);
	}


	getToken
	if (token->type == SUB || token->type == ADD || token->type == MULT ||\
			 token->type == DIV){
		rule_op(token, sucess);
		getToken
		rule_expr(token,keyWords,sucess);
	}
}

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
	}
}	
//<var asg> - <id_n> = <values>
void rule_var_asg(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid ,")
		rule_id_n(token,keyWords,sucess);
	}
	print_debug("valid =")
	getToken
	rule_values(token,keyWords,sucess);
}
//<values> - <expr> <expr_n>
//<values> - id <func_call>
//<values> - id <op> <expr>    --> <expr> - <expr> <op> <expr>
void rule_values(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == OBR || token->type ==INT_L || token->type == FLOAT_L ||\
			token->type == STRING_L){
		rule_expr(token,keyWords,sucess);
		getToken
		rule_expr_n(token,keyWords,sucess);
	}else if (token->type == ID){
		print_debug("valid ID")
		getToken
		if (token->type == OBR){
			print_debug("valid (")
			getToken
			rule_func_call(token,keyWords,sucess);
		}else if(token->type == ADD || token->type == SUB || token->type == MULT || \
				token->type == DIV){
			rule_op(token, sucess);
			getToken
			rule_expr(token,keyWords,sucess);
		//	getToken
			rule_expr_n(token,keyWords,sucess);
		}else{
			*sucess = 0;
		}
	}
}

//<expr_n> - , <expr> <expr_n>
//<expr_n> - epsilon
void rule_expr_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid ,")
		getToken
		rule_expr(token,keyWords,sucess);
        getToken
        rule_expr_n(token,keyWords,sucess);
	}
}

//<id_n> - , id <id_n> 
//<id_n> - , _ <id_n>
//<id_n> - epsilon
void rule_id_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		getToken
		if (token->type == ID || token->type == US){
			print_debug("valid ID or _")
			getToken
			rule_id_n(token,keyWords,sucess);
		}else{
			*sucess = 0;
		}
	}
}

// <if> - if <expr> <expr_bool> { <body> } <else>
void rule_if(tToken *token, tKWPtr keyWords, bool* sucess){
	getToken
	rule_expr(token,keyWords,sucess);
	if (token->type != OB){
		rule_expr_bool(token,keyWords,sucess);
	}

	if (token->type == OB){
		print_debug("valid {")
		getToken
		rule_body(token,keyWords,sucess);
		if (token->type == CB){
			print_debug("valid }")
			getToken
		}else{
			*sucess = 0;
		}
	}else{
		*sucess = 0;
	}
	rule_else(token,keyWords,sucess);
}

// <expr_bool> - <bool_op> <expr>
void rule_expr_bool(tToken *token, tKWPtr keyWords, bool* sucess){
	rule_bool_op(token,keyWords,sucess);
	rule_expr(token,keyWords,sucess);
	
}


//<else> - else { <body> }
 void rule_else(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == KW_ELSE)	{
		print_debug("valid KW_ELSE")
		getToken
		if ( token->type == OB){
			print_debug("valid {")
			getToken
			rule_body(token,keyWords,sucess);
			if (token->type == CB){
				print_debug("valid }")
				getToken
			}else{
				*sucess = 0;
			}
		}else{
			*sucess = 0;
		}
	}
}


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
	}
	getToken
}

//<for> - for <var_def_cycle>; <expr_bool>; <asg_opt> { <body> }
//<var_def_cycle> - ID <var_def>
//<var_def_cycle> - epsilon
//<asg_opt> - <expr> = <expr>
//<expr_opt> - epsilon
void rule_for(tToken *token, tKWPtr keyWords, bool* sucess){
	getToken
	if (token->type == ID){
		print_debug("valid ID")
		getToken
		print_debug("valid :=")
		rule_var_def(token,keyWords,sucess);
	}else if (token->type == SEM){
		print_debug("valid ;")
	}else{
		*sucess = 0;
	}
	getToken
	rule_expr(token,keyWords,sucess);
	rule_expr_bool(token,keyWords,sucess);
	if (token->type == SEM){
		print_debug("valid ;")
	}else{
		*sucess = 0;
	}
	getToken

	if (token->type != OB){
		rule_expr(token,keyWords,sucess);
		if (token->type == ASG){
			print_debug("valid =")
		}else{
			*sucess = 0;
		}
		getToken
		rule_expr(token,keyWords,sucess);
	}
	if (token->type == OB){
		print_debug("valid {")
		getToken
		rule_body(token,keyWords,sucess);
		
	}else{
		*sucess = 0;
	}
	if (token->type == CB){
		print_debug("valid }")
		getToken
	}else{
		*sucess = 0;
	}
}


void rule_params(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == ID){
		print_debug("valid ID")
		getToken
		rule_type(token,keyWords,sucess);
		rule_params_n(token,keyWords,sucess);
	}
}



void rule_params_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid , ")
		getToken
		rule_params(token,keyWords,sucess);
	}

}
//<type> - int
//<type> - float64
//<type> - string
//<type> - epsilon
void rule_type(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == KW_INT){
		print_debug("valid KW_INT")
	}else if (token->type == KW_FLOAT64){
		print_debug("valid KW_FLOAT64")
	}else if (token->type == KW_STRING){
		print_debug("valid KW_STRING")
	}else{
		//epsilon
		return;
	}
	getToken
}

void rule_type_n(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == COM){
		print_debug("valid , ")
		getToken
		rule_type(token,keyWords,sucess);
		rule_type_n(token,keyWords,sucess);
	}
}

//<return_types> - (<types> <types_n>)
//<return_types> -
void rule_return_type(tToken *token, tKWPtr keyWords, bool* sucess){
	if (token->type == OBR){
		print_debug("valid (")
		getToken
		rule_type(token,keyWords,sucess);
		rule_type_n(token,keyWords,sucess);
		if (token->type == CBR){
			print_debug("valid )")
			getToken
		}else{
			*sucess = 0;
		}
	}
}

