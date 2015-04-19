#include "util.h"
#include "symbol.h"
#include "types.h"
#include "env.h"

E_enventry E_VarEntry(Ty_ty ty){
  E_enventry e = checked_malloc(sizeof(*e));
  e->kind = E_varEntry;
  e->u.var.ty = ty;
  return e;
}
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result){
  E_enventry e = checked_malloc(sizeof(*e));
  e->kind = E_funEntry;
  e->u.fun.formals = formals;
  e->u.fun.result = result;
  return e;
}


S_table E_base_tenv(){
  S_table tbl = S_empty();
  // predefine type env for int and string
  S_enter(tbl, S_Symbol("int"), Ty_Int());
  S_enter(tbl, S_Symbol("string"), Ty_String());
  return tbl;
}

S_table E_base_venv(){
  // haven't done add predefine funcitons
  return S_empty();
}

