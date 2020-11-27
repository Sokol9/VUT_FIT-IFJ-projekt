#ifndef SYNTAX_H
#define SYNTAX_H

#include "project.h"


 
//#define DEBUG
#define GET_TOKEN getToken(token,  keyWords);
  
// EOL_REQUIRED kontroluje, ci pred aktualne nacitanim tokenom bol EOL, vracia chybu, ak nebol
#define EOL_REQUIRED if (!token->eolFlag) if (token->type != TOKEN_EOF) setError(SYN_ERROR);
 
//  EOL_FORBID zakazuje odriadkovanie, v pripade porusenia, nastavy error
#define EOL_FORBID if (token->eolFlag) setError(SYN_ERROR);
 
// v pripade chyby sa zastav=y kontrola tokenov, az po miesto CHECK_POINT, kde vieme aky token
// nasleduje, a vsetky ostatne zahadzujeme, pripadne koncime na EOF
#define CHECK_POINT(tp,fsm) \
    if (!*sucess){\
		setError(SYN_ERROR);\
        while (token->tp != fsm && token->type !=TOKEN_EOF){\
            GET_TOKEN\
            /*printf("--token:%d\n",*sucess);*/\
        }\
        if(token->tp == fsm) *sucess = 1; else return;\
    }

#ifdef DEBUG
    #define print_debug(fsm) \
            printf("%s, sucess: %d - line:%d - %s\n",\
            fsm, *sucess,__LINE__,  __func__);
    #define printd(fsm) \
            printf("INVALID TOKEN, i needed %s, sucess: %d - line:%d -%s\n",\
            fsm, *sucess, __LINE__, __func__);
#else
    #define print_debug(fsm)
	#define printd(fsm)
#endif







/*================================================================
 * funkcie rule, reprezentuju gramatiku jazyka nevracaju ziadnu hodnotu
 ================================================================*/

void rule_prog(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_func_def(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_params(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_return_type(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_body(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_stat(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_term(tToken *token, bool* sucess);
void rule_term_n(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_func_call(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_func_def(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_var_def(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_expr(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_op(tToken *token, bool* sucess);
void rule_expr_n(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_id_n(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_var_asg(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_values(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_if(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_expr_bool(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_bool_op(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_else(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_for(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_params(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_params_n(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_type(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_type_n(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_return_type(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_func_n(tToken *token, tKWPtr keyWords, bool* sucess);


#endif //SYNTAX_H
