/*
 * env.h for the missing file in tiger book,
 * maybe has error, but a good pratice,
 * haha.
 * */
typedef struct E_enventry_ *E_enventry;

struct E_enventry_ {
  enum {E_varEntry, E_funEntry} kind;
  union{
    struct {Ty_ty ty;} var;
    struct {Ty_tyList formals; Ty_ty result;} fun;
  } u;
};

E_enventry E_VarEntry(Ty_ty ty);
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result);

S_table E_base_tenv(); // base type enviroment
S_table E_base_venv(); // base variable enviroment
