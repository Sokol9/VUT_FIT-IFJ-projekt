#ifndef SYNTAX_H
#define SYNTAX_H

#include "project.h"
#include "symtable.h"
 
#define DEBUG_INVALID
#define GET_TOKEN getToken(token,  keyWords);
  
// EOL_REQUIRED kontroluje, ci pred aktualne nacitanim tokenom bol EOL, vracia chybu, ak nebol
#define EOL_REQUIRED if (!token->eolFlag) if (token->type != TOKEN_EOF) setError(SYN_ERROR);
 
//  EOL_FORBID zakazuje odriadkovanie, v pripade porusenia, nastavy error
#define EOL_FORBID if (token->eolFlag) setError(SYN_ERROR);
 
// v pripade chyby sa zastav=y kontrola tokenov, az po miesto CHECK_POINT, kde vieme aky token
// nasleduje, a vsetky ostatne zahadzujeme, pripadne koncime na EOF
#define CHECK_POINT(tp,fsm) \
    if (!*sucess){\
        while (token->tp != fsm && token->type !=TOKEN_EOF){\
            GET_TOKEN\
            printf("--token:%d\n",*sucess);\
        }\
        if(token->tp == fsm) *sucess = 1; else return;\
    }

#define PARAMS token, STab, keyWords, sucess

#ifdef DEBUG_VALID
    #define print_debug(fsm) \
            printf("%s, sucess: %d - line:%d - %s\n",\
            fsm, *sucess,__LINE__,  __func__);
#else
    #define print_debug(fsm)
#endif

#ifdef DEBUG_INVALID
    #define printd(fsm) \
            printf("INVALID TOKEN, i needed %s, sucess: %d - line:%d -%s\n",\
            fsm, *sucess, __LINE__, __func__);
#else
    #define printd(fsm)
#endif

typedef enum  {RULE_PARAM, RULE_RET, RULE_FCALL, RULE_EXPR}callAs;



/*================================================================
 * funkcie rule, reprezentuju gramatiku jazyka nevracaju ziadnu hodnotu
 ================================================================*/

void rule_prog(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_func_def(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_params(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_body(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_stat(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_term(tToken *token, tSymTablePtr STab, bool* sucess, callAs call);
void rule_term_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_func_call(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_func_def(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_var_def(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_expr(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_op(tToken *token, bool* sucess);
void rule_expr_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_id_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tRetListPtr ret);
void rule_var_asg(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_values(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_if(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_expr_bool(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_bool_op(tToken *token, tKWPtr keyWords, bool* sucess);
void rule_else(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_for(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_params(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_params_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_type(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_type_r(tToken *token, tKWPtr keyWords, bool* sucess, tRetListPtr ret);
void rule_type_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess, tRetListPtr ret);
void rule_return_type(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);
void rule_func_n(tToken *token, tSymTablePtr STab, tKWPtr keyWords, bool* sucess);


#endif //SYNTAX_H
