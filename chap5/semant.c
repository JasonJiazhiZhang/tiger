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
  if(!ty) assert(0); //should not reach here, ty should not be null
  if (ty->kind == Ty_name){
    return actual_ty(ty->u.name.ty);
  } else {
    return ty;
  }
}

struct expty transVar(S_table venv, S_table tenv, A_var v){
  switch (v->kind){
  case A_simpleVar:{
    E_enventry e = S_look(venv, v->u.simple);
    if (e && e->kind == E_varEntry)
      return expTy(NULL, actual_ty(e->u.var.ty));
    else {
      EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
      return expTy(NULL, Ty_Nil());
    }
  }
  case A_fieldVar:{
    struct expty et = transVar(venv, tenv, v->u.field.var);
    if (et.ty->kind != Ty_record){
      EM_error(v->pos, "record type needed");
      return expTy(NULL, Ty_Nil());
    }
    
    Ty_fieldList h;
    for(h = et.ty->u.record; h; h = h->tail){
      if(h->head->name == v->u.field.sym){
	return expTy(NULL, actual_ty(h->head->ty));
      }
    }
    EM_error(v->pos, "not such field in record");
    return expTy(NULL, Ty_Nil());
  }
  case A_subscriptVar:{
    struct expty et = transExp(venv, tenv, v->u.subscript.exp);
    if(et.ty->kind != Ty_int){
      EM_error(v->pos, "index should be int");
      return expTy(NULL, Ty_Nil());
    }
    struct expty et = transVar(venv, tenv, v->u.subscript.var);
    if(et.ty->kind != Ty_array){
      EM_error(v->pos, "array type needed");
      return expTy(NULL, Ty_Nil());
    }
    return expTy(NULL, actual_ty(et.ty->u.array));
  }
  }
    
  assert(0);
}


struct expty transExp(S_table venv, S_table tenv, A_exp a){

  switch (a->kind){
  case A_varExp:
    return transVar(venv, tenv, a->u.var);
  case A_nilExp:
    return expTy(NULL, Ty_Nil());
  case A_intExp:
    return expTy(NULL, Ty_Int());
  case A_stringExp:
    return expTy(NULL, Ty_String());
  case A_callExp:{
    E_enventry e = S_look(venv, a->u.call.func);
    if(!e || e->kind != E_funEntry){
      EM_error(a->pos, "not a function");
      return expTy(NULL, Ty_Nil()); 
    }
    Ty_tyList tyl;
    A_expList al;

    for (tyl = e->u.fun.formals, al = a->u.call.args;
	 tyl && al;
	 tyl = tyl->tail, al = al->tail) {
      struct expty et = transExp(venv, tenv, al->head);
      if(et.ty != actual_ty(tyl->head)){
	EM_error(al->head->pos, "type not right of func argument");
	return expTy(NULL, Ty_Nil()); 
      }
    }
     
    if(tyl || al){ // tyl and al is not the same length
      EM_error(a->pos, "argument length not right");
      return expTy(NULL, Ty_Nil()); 
    }
    return expTy(NULL, actual_ty(e->u.fun.result));
  }
    
  case A_opExp:{ // simple check the left should be the same type with right
    struct expty et_left = transExp(venv, tenv, a->u.op.left);
    struct expty et_right = transExp(venv, tenv, a->u.op.right);
    if(et_left.ty != et_right.ty){
      EM_error(a->pos, "type should be the same for left and right");
      return expTy(NULL, Ty_Nil()); 
    }
    return expTy(NULL, et_left.ty);
  }
  case A_recordExp: {
    Ty_ty recordType = actual_ty(S_look(tenv, a->u.record.typ));
    if(recordType->kind != Ty_record){
      EM_error(a->pos, "type is not record type");
      return  expTy(NULL, Ty_Nil());
    }
    Ty_fieldList list = NULL;
    Ty_field field = NULL;
    int a_num= 0, ty_num = 0;
    A_efieldList ls;
    Ty_fieldList tls;
    for(ls = a->u.record.fields; ls; ls = ls->tail){
      a_num++;
      for(tls = recordType->u.record; tls; tls = tls->tail){
	if(ls->head->name == tls->head->name){//same type here
	  struct expty et = transExp(venv, tenv, ls->head->exp);
	  if(actual_ty(tls->head->ty) != et.ty){
	    EM_error(a->pos, "type should be the same");
	    return  expTy(NULL, Ty_Nil());
	  }
	  ty_num++;
	}
      }
    }
    if(a_num != ty_num){
      EM_error(a->pos, "type num should same");
      return  expTy(NULL, Ty_Nil());
    }
    return expTy(NULL, recordType);
  }
  case A_seqExp:{
    A_expList l = a->u.seq;
    if(!l){
      return expTy(NULL, Ty_Nil());
    }
    while(l->tail){
      l = l->tail;
    }
    return transExp(venv, tenv, l->head);
  }
  case A_assignExp:{
    struct expty et_left = transVar(venv, tenv, a->u.assign.var);
    struct expty et_right = transExp(venv, tenv, a->u.assign.exp); 
    if(et_left.ty != et_right.ty){
      EM_error(a->pos, "assign left type not same with right side");
      return  expTy(NULL, Ty_Nil());
    }
    return expTy(NULL, Ty_Void());
  }

  case A_breakExp:
    return expTy(NULL, Ty_Void());
  case A_ifExp:{
    struct expty et_test = transExp(venv, tenv, a->u.iff.test);
    if(et_test.ty != Ty_Int()){
      EM_error(a->pos, "type of if should be int");
      return  expTy(NULL, Ty_Nil());
    }
    struct expty et_then = transExp(venv, tenv, a->u.iff.then);

    if(a->u.iff.elsee){ // we have else clouse
      struct expty et_else = transExp(venv, tenv, a->u.iff.elsee);
      if(et_then.ty != et_else.ty){
	EM_error(a->pos, "type of then should be the same else");
	return  expTy(NULL, Ty_Nil());
      }
    }
    return expTy(NULL, et_then.ty);
  }

  case A_whileExp:{
    struct expty et_test = transExp(venv, tenv, a->u.whilee.test);
    if(et_test.ty != Ty_Int()){
      EM_error(a->pos, "type of while should be int");
      return expTy(NULL, Ty_Nil());
    }
    struct expty et_body = transExp(venv, tenv, a->u.whilee.body);
    return expTy(NULL, et_body.ty);
  }
      
  case A_forExp:{
    struct expty lo = transExp(venv, tenv, a->u.forr.lo);
    struct expty hi = transExp(venv, tenv, a->u.forr.hi);
    struct expty body;
    if (lo.ty != Ty_Int() || hi.ty != Ty_Int()) {
      EM_error(a->pos, "low or high range type is not integer");
      return expTy(NULL, Ty_Nil());
    }
    /* type check only, don't need for now
    S_beginScope(venv);
    transDec(venv, tenv, A_VarDec(a->pos, a->u.forr.var, S_Symbol("int"), a->u.forr.lo));
    S_endScope(venv);
    */
    return expTy(NULL, Ty_Void());
  }

  case A_letExp:{
    struct expty final;
    A_decList d;
    S_beginScope(venv);
    S_beginScope(tenv);
    for (d = a->u.let.decs; d; d = d->tail) {
      transDec(venv, tenv, d->head);
    }
    final = transExp(venv, tenv, a->u.let.body);
    S_endScope(venv);
    S_endScope(tenv);
    return final;
  }

  case A_arrayExp:{
    Ty_ty type = actual_ty(S_look(tenv, a->u.array.typ));
    if(!type || type->kind != Ty_array){
      EM_error(a->pos, "not an arraytype");
      return expTy(NULL, Ty_Nil());
    }
    struct expty et_size = transExp(venv, tenv, a->u.array.size);
    if(et_size.ty != Ty_Int()){
      EM_error(a->pos, "not an int type");
      return expTy(NULL, Ty_Nil());
    }
    struct expty et_init = transExp(venv, tenv, a->u.array.init);
    if(et_init.ty != type->u.array){
      EM_error(a->pos, "init type not match array type");
      return expTy(NULL, Ty_Nil());
    }
    return expTy(NULL, type);
    
  }

  }
  assert(0);
}
void transDec(S_table venv, S_table tenv, A_dec d){
  // TO-DO: make it recursice
  switch(d->kind){
  case A_varDec:{
    struct expty e = transExp(venv, tenv, d->u.var.init);
    if (d->u.var.typ){ // if type specified
      Ty_ty var_type = S_look(tenv, d->u.var.typ);
      if (var_type != e.ty){
	EM_error(d->pos, "type different between left side and right side");
	return expTy(NULL, Ty_Nil());
      }
    }
    S_enter(venv, d->u.var.var, E_VarEntry(e.ty));
    return;
  }
  case A_typeDec:{
    A_nametyList ls = NULL;
    for (ls = d->u.type; ls; ls=ls->tail) {
      S_enter(tenv, ls->head->name, transTy(tenv, ls->head->ty));
    }
    return;
  }
  case A_functionDec:{
    A_fundecList ls;
    for(ls = d->u.function; ls; ls=ls->tail){
      A_fieldList fields = ls->head->params;
      // trans fields to formals
      Ty_tyList formals = NULL;
      for(; fields; fields = fields->tail) {
	A_field param = fields->head; 
	Ty_ty param_ty = actual_ty(S_look(tenv, param->typ));
	formals = Ty_TyList(param_ty, formals);
      }
      // check result type ?
      Ty_ty rt_ty = actual_ty(S_look(tenv, ls->head->result));
      S_enter(venv, ls->head->name, E_FunEntry(formals, rt_ty));
      
      S_beginScope(venv);
      {
	A_fieldList l; Ty_tyList t;
	for(l = fields, t = formals; l; l=l->tail, t=t->tail){
	  S_enter(venv, l->head->name, E_VarEntry(t->head));
	}
	transExp(venv, tenv, ls->head->body);
      }
      S_endScope(venv);
    }
    return;
  }
    
  }
}

Ty_ty transTy ( S_table tenv, A_ty a){
  // TO-DO: make it recursive
  switch(a->kind){
  case A_nameTy:{
    Ty_ty ty = actual_ty(S_look(tenv, a->u.name));
    if(!ty){
      EM_error(a->pos, "undefined type");
      return Ty_Nil();
    }
    return ty;
  }
  case A_recordTy:{
    A_fieldList ls;
    Ty_fieldList ty_ls = NULL;
    for(ls = a->u.record; ls; ls = ls->tail){
      A_field record = ls->head;
      Ty_ty ty = actual_ty(S_look(tenv, record->typ));
      if(!ty){
	EM_error(record->pos, "undefined type");
	return Ty_Nil();
      }
      Ty_field t = Ty_Field(record->name, ty);
      ty_ls = Ty_FieldList(t, ty_ls);
    }
    
    return Ty_Record(ty_ls);
  }
  case A_arrayTy:{
    Ty_ty ty = actual_ty(S_look(tenv, a->u.array));
    if(!ty){
      EM_error(a->pos, "undefined type");
      return Ty_Nil();
    }
    return Ty_Array(ty);
  }
  
  }

  assert(0);
}
