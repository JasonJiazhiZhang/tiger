#include "slp.h"
#include "prog1.h"
#include "util.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("test: %d\n", maxargs(prog()));
    Table_ t1 = Table("a", 1 , NULL);
    Table_ t2 = Table("b", 1 , t1);
    Table_ t = intepStm(prog(), NULL);
    printf("a: %d\n", lookup(t, "a"));
    printf("b: %d\n", lookup(t, "b"));


    Table_ i = t;
    for (; i != NULL; i = i->tail){
        printf("%s, %d\n", i->id, i->value);
    }
    return 0;
}

