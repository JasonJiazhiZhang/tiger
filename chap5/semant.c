#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "table.h"
#include "absyn.h"
#include "types.h"
#include "env.h"
#include "semant.h"

Ty_ty actual_ty(Ty_ty ty){
  if(!ty) assert(0); //should not reach here
  if (ty->kind == Ty_name){
    return actual_ty(ty->u.name.ty);
  } else {
    return ty;
  }
}

struct expty transVar(S_table venv, S_table tenv, A_var v){
  E_enventry e;
  struct expty et;
  Ty_fieldList h;

  switch (v->kind){
    case A_simpleVar: 
      e = S_look(venv, v->u.simple);
      if (e && e->kind == E_varEntry)
        return expTy(NULL, actual_ty(e->u.var.ty));
      else {
        EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
        return expTy(NULL, Ty_Nil());
      }

    case A_fieldVar:
      et = transVar(venv, tenv, v->u.field.var);
      if (et.ty->kind != Ty_record){
        EM_error(v->pos, "record type needed");
        return expTy(NULL, Ty_Nil());
      }
      for(h = et.ty->u.record; h; h = h->tail){
        if(h->head->name == v->u.field.sym){
          return expTy(NULL, actual_ty(h->head->ty));
        }
      }
      EM_error(v->pos, "not such field in record");
      return expTy(NULL, Ty_Nil());

    case A_subscriptVar:
      et = transExp(venv, tenv, v->u.subscript.exp);
      if(et.ty->kind != Ty_int){
        EM_error(v->pos, "index should be int");
        return expTy(NULL, Ty_Nil());
      }
      et = transVar(venv, tenv, v->u.subscript.var);
      if(et.ty->kind != Ty_array){
        EM_error(v->pos, "array type needed");
        return expTy(NULL, Ty_Nil()); }
      return expTy(NULL, actual_ty(et.ty->u.array));
  }

  assert(0);
}
struct expty transExp(S_table venv, S_table tenv, A_exp a){
  E_enventry e;
  Ty_tyList tyl;
  A_expList al;
  struct expty et;
  struct expty et2;

  switch (a->kind){
    case A_varExp:
      return transVar(venv, tenv, a->u.var);
    case A_nilExp:
      return expTy(NULL, Ty_Nil());
    case A_intExp:
      return expTy(NULL, Ty_Int());
    case A_stringExp:
      return expTy(NULL, Ty_String());
    case A_callExp:
      // TO-DO
      e = S_look(venv, a->u.call.func);
      if(e && e->kind == E_funEntry){
        for (tyl = e->u.fun.formals, al = a->u.call.args;
             tyl && al;
             tyl = tyl->tail, al = al->tail)
        {
          et = transExp(venv, tenv, al->head);
          if(et.ty != actual_ty(tyl->head)){
            EM_error(al->head->pos, "type not right of func argument");
            return expTy(NULL, Ty_Nil()); 
          }
        }
        // after end of for
        if(tyl || al){ // tyl and al is not the same length
          EM_error(a->pos, "argument length not right");
          return expTy(NULL, Ty_Nil()); 
        } else{
          return expTy(NULL, actual_ty(e->u.fun.result));
        }
      } else{
        EM_error(a->pos, "not a function");
        return expTy(NULL, Ty_Nil()); 
      }
    case A_opExp: // simple check the left should be the same
                  // type with right
      et = transExp(venv, tenv, a->u.op.left);
      et2 = transExp(venv, tenv, a->u.op.right);
      if(et.ty != et2.ty){
        EM_error(a->pos, "type should be the same for left and right");
        return expTy(NULL, Ty_Nil()); 
      }
      return expTy(NULL, et.ty);

    // TO-continue case A_recordExp:




  }
  assert(0);
}
void transDec(S_table venv, S_table tenv, A_dec d){
  // TO-DO
}

Ty_ty transTy ( S_table tenv, A_ty a){
  // TO-DO
  return Ty_Nil();
}
