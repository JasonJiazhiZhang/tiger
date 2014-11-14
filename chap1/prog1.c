#include "util.h"
#include "slp.h"
#include "prog1.h"

A_stm prog(void) {

return 
 A_CompoundStm(A_AssignStm("a",
                 A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),
  A_CompoundStm(A_AssignStm("b",
      A_EseqExp(A_PrintStm(A_PairExpList(A_IdExp("a"),
                 A_LastExpList(A_OpExp(A_IdExp("a"), A_minus, 
                                       A_NumExp(1))))),
              A_OpExp(A_NumExp(10), A_times, A_IdExp("a")))),
   A_PrintStm(A_LastExpList(A_IdExp("b")))));
}

// the 1 question
int maxargs(A_stm stm){
    switch (stm->kind) {
        case A_compoundStm:
            return max(maxargs(stm->u.compound.stm1), maxargs(stm->u.compound.stm2));
            break;
        case A_assignStm:
            return maxargs_exp(stm->u.assign.exp);
            break;
        case A_printStm:
            return max(len(stm->u.print.exps),maxargs_exps(stm->u.print.exps));
            break;
        default:
            assert(FALSE);
            break;
    }
}

int maxargs_exp(A_exp e){
    switch (e->kind) {
        case A_opExp:
            return max(maxargs_exp(e->u.op.left), maxargs_exp(e->u.op.right));
            break;
        case A_eseqExp:
            return max(maxargs(e->u.eseq.stm), maxargs_exp(e->u.eseq.exp));
            break;
        default:
            return 0;
            break;
            
    }
}

int maxargs_exps(A_expList es){
    switch (es->kind) {
        case A_pairExpList:
            return max(maxargs_exp(es->u.pair.head), maxargs_exps(es->u.pair.tail));
            break;
        case A_lastExpList:
            return maxargs_exp(es->u.last);
            break;
        default:
            assert(FALSE);
            break;

    }
}

int len(A_expList es){
    switch (es->kind) {
        case A_pairExpList:
            return 1 + len(es->u.pair.tail);
            break;
        case A_lastExpList:
            return 1;
            break;
        default:
            assert(FALSE);
            break;

    }
}

// here comes the second
int lookup(Table_ t, string key){
    for (Table_ i = t; t != NULL ; i = i.tail)
    {
        if(strcmp(i->id, key)){
            return i->value;
        }
    }
    assert(FALSE);
}

Table_ intepStm(A_stm s, Table_ t){
    switch (s->kind){
        case A_compoundStm:
            return intepStm(s->u.compound.stm2,intepStm(s->u.compound.stm1, t));
            break;
        case A_assignStm:
            return maxargs_exp(stm->u.assign.exp);
            break;
        case A_printStm:
            return max(len(stm->u.print.exps),maxargs_exps(stm->u.print.exps));
            break;
        default:
            assert(FALSE);
            break;
    }
}
