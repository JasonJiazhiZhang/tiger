#include <stdlib.h>
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
Table_ Table(string id, int value, Table_ tail){
    Table_ t = checked_malloc(sizeof(struct table));
    t->id = id;
    t->value = value;
    t->tail = tail;
    return t;
}

int lookup(Table_ t, string key){
    Table_ i = t;
    for (; t != NULL ; i = i->tail)
    {
        if(strcmp(i->id, key) == 0){
            return i->value;
        }
    }
    assert(FALSE);
}


Table_ intepStm(A_stm s, Table_ t){
    struct IntAndTable it;
    switch (s->kind){
        case A_compoundStm:
            return intepStm(s->u.compound.stm2,intepStm(s->u.compound.stm1, t));
            break;
        case A_assignStm:
            it = intepExp(s->u.assign.exp,t);
            return Table(s->u.assign.id, it.i, it.t);
            break;
        case A_printStm:
            return intepExps(s->u.print.exps, t);
            break;
        default:
            assert(FALSE);
            break;
    }
}

struct IntAndTable intepExp(A_exp e, Table_ t){
    struct IntAndTable it_left, it_right, result;
    Table_ tt;
    int value;

    switch (e->kind){
        case A_numExp:
            return (struct IntAndTable){e->u.num, t};
            break;
        case A_idExp:
            return (struct IntAndTable){lookup(t, e->u.id), t};
            break;
        case A_opExp:
            it_left = intepExp(e->u.op.left, t);
            it_right = intepExp(e->u.op.right, it_left.t);
            value = cal_(e->u.op.oper ,it_left.i, it_right.i);
            return (struct IntAndTable){value, it_right.t};
            break;
        case A_eseqExp:
            tt = intepStm(e->u.eseq.stm, t);
            return intepExp(e->u.eseq.exp, tt);
            break;
        default:
            assert(FALSE);
            break;
    }
}

int cal_(A_binop op, int left, int right){
    switch (op) {
        case A_plus:
            return left+right;
            break;
        case A_minus:
            return left-right;
            break;
        case A_times:
            return left*right;
            break;
        case A_div:
            return left / right;
            break;
        default:
            assert(FALSE);
            break;
    }
}

Table_ intepExps(A_expList es, Table_ t){
    struct IntAndTable it;
    switch (es->kind) {
        case A_pairExpList:
            it = intepExp(es->u.pair.head, t);
            return intepExps(es->u.pair.tail, it.t);
            break;
        case A_lastExpList:
            it = intepExp(es->u.last, t);
            return it.t;
            break;
        default:
            assert(FALSE);
            break;
    }
}
