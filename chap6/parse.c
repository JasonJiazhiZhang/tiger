/*
 * parse.c - Parse source file.
 */

#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"
#include "types.h"
#include "semant.h"
#include "env.h"

extern int yyparse(void);
extern A_exp absyn_root;

/* parse source file fname; 
   return abstract syntax data structure */
A_exp parse(string fname) 
{EM_reset(fname);
  if (yyparse() == 0) /* parsing worked */
    return absyn_root;
  else return NULL;
}

int main(int argc, char **argv) {
  if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); return 1;}

  FILE* output = fdopen(1, "w");
 
  if(!output){
    fprintf(stderr,"open file to write error\n");
    return 2;
  }
  
  A_exp exp = parse(argv[1]);
  pr_exp(output, exp, 0);

  struct expty et = transExp(E_base_venv(), E_base_tenv(),exp);
  
  fprintf(output, "the type check ok: %d", et.ty->kind);
  

  return 0;
}
