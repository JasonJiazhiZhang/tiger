a.out: parse.o y.tab.o lex.yy.o errormsg.o util.o table.o absyn.o prabsyn.o symbol.o
	cc -g parse.o y.tab.o lex.yy.o errormsg.o util.o table.o absyn.o prabsyn.o symbol.o

parse.o: parse.c errormsg.h util.h
	cc -g -c parse.c

symbol.o: symbol.c util.h table.h
	cc -g -c symbol.c

table.o: table.c util.h table.h
	cc -g -c table.c

types.o: types.c types.h
	cc -g -c types.c

env.o: env.c env.h
	cc -g -c env.c

semant.o: semant.c semant.h
	cc -g -c semant.c

absyn.o: absyn.c util.h symbol.h
	cc -g -c absyn.c

prabsyn.o: prabsyn.c util.h symbol.h absyn.h
	cc -g -c prabsyn.c

y.tab.o: y.tab.c
	cc -g -c y.tab.c

y.tab.c: tiger.grm
	yacc -dv tiger.grm

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
	cc -g -c lex.yy.c

lex.yy.c: tiger.lex
	flex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f a.out util.o parsetest.o lex.yy.o errormsg.o y.tab.c y.tab.h y.tab.o *.o
