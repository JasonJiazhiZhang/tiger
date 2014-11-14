#ifndef util_h_once
#define util_h_once

#include <assert.h>

#define max(a,b) ((a)>(b)?(a):(b))

typedef char *string;
typedef char bool;

#define TRUE 1
#define FALSE 0

void *checked_malloc(int);
string String(char *);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);

#endif
