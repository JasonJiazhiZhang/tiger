#ifndef prog_h_once
#define prog_h_once
typedef struct table *Table_;
struct table {string id; int value; Table_ tail;};
struct IntAndTable {int i; Table_ t;};

Table_ Table(string id, int value, Table_ tail);

A_stm prog(void);
int maxargs(A_stm stm);
int maxargs_exp(A_exp e);
int len(A_expList es);
int maxargs_exps(A_expList es);
Table_ intepExps(A_expList es, Table_ t);
int cal_(A_binop op, int left, int right);
struct IntAndTable intepExp(A_exp e, Table_ t);
Table_ intepStm(A_stm s, Table_ t);
int lookup(Table_ t, string key);
#endif
