%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

string handle_string(string str){
    int len = strlen(str) - 1;
    string result = checked_malloc(len);
    result[len - 1] = '\0';

    int i = 0, j = 1;
    while (str[j] != '"'){
        if (str[j] == '\\'){
            j++;
            switch (str[j]){
                case 'n':
                    result[i] = '\n';
                    i++; j++;
                    break;
                case 't':
                    result[i] = '\t';
                    i++; j++;
                    break;
                case '\\':
                    result[i] = '\\';
                    i++; j++;
                    break;
                case '\n': case '\t': case ' ':
                    j++;
                    while (str[j] != '\\');
                    j++;
                    break;
                default:
                    if (j + 2 <= len &&
                        str[j] >= '0' && str [j] <= '9' && 
                        str[j+1] >= '0' && str [j+1] <= '9'
                        && str[j+2] >= '0' && str [j+2] <= '9'){
                            result[i] = atoi(str+j);
                            i++;
                            j += 3;
                    } else {
                        result[i] = str[j];
                        i++; j++;
                    }
                    break;
            }
        }
        else{
            result[i] = str[j];
            i++; j++;
        }
    }
    result[i] = '\0';
    return result;
}


%}

digit [0-9]
alpha [a-zA-Z]
alphanum [a-zA-Z0-9]
ID [a-zA-Z][a-zA-Z0-9_]*
EMPTY [ \t]+

%x comment

%%
    int in_comment = 0;
{EMPTY}	 {adjust(); continue;}
<comment,INITIAL>\n	 {adjust(); EM_newline(); continue;}
","	 {adjust(); return COMMA;}
"("  {adjust(); return LPAREN;}
")"  {adjust(); return RPAREN;}
"["  {adjust(); return LBRACK;}
"]"  {adjust(); return RBRACK;}
"{"  {adjust(); return LBRACE;}
"}"  {adjust(); return RBRACE;}
"."  {adjust(); return DOT;}
"+"  {adjust(); return PLUS;}
"-" {adjust(); return MINUS;}
"*" {adjust(); return TIMES;} 
"/" {adjust(); return DIVIDE;} 
"==" {adjust(); return EQ;}
"!=" {adjust(); return NEQ;}
"<" {adjust(); return LT;}
"<=" {adjust(); return LE;}
">" {adjust(); return GT;}
">=" {adjust(); return GE;}
"&&" {adjust(); return AND;}
"||" {adjust(); return OR;}
"=" {adjust(); return ASSIGN;}
"array" {adjust(); return ARRAY;}
"if" {adjust(); return IF;}
"then" {adjust(); return THEN;}
"else" {adjust(); return ELSE;}
"while" {adjust(); return WHILE;}
"for" {adjust(); return FOR;}
"to" {adjust(); return TO;}
"do" {adjust(); return DO;}
"let" {adjust(); return LET;}
"in" {adjust(); return IN;}
"end" {adjust(); return END;}
"of" {adjust(); return OF;}
"break" {adjust(); return BREAK;}
"nil" {adjust(); return NIL;}
"function" {adjust(); return FUNCTION;}
"var" {adjust(); return VAR;}
"type" {adjust(); return TYPE;}
":" {adjust(); return COLON;}
";" {adjust(); return SEMICOLON;}
{digit}+	 {adjust(); yylval.ival=atoi(yytext); return INT;}
{ID} {adjust(); yylval.sval = String(yytext); return ID;}
"/*" {adjust(); BEGIN(comment); in_comment = 1;}
<comment>"/*" {adjust(); in_comment += 1; continue;}
<comment>"*/" {adjust(); in_comment -= 1; if (in_comment == 0) BEGIN(INITIAL); continue;}
<comment>. {adjust(); continue;}
\"(\\.|[^\\"])*\" {adjust(); yylval.sval=handle_string(yytext); return STRING;}
.	 {adjust(); EM_error(EM_tokPos,"illegal token");}



