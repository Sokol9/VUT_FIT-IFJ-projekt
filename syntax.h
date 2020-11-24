#ifndef SYNTAX_H
#define SYNTAX_H

#include "project.h"
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
