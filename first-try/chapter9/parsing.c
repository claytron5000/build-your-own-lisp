#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

/* If we are compiling on Windows */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer)+1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy)-1] = '\0';
	return cpy;
}

/* Fake history */
void add_history(char* unused) {}

/* Otherwise inlcude the editline headers */
#else
#include <editline/readline.h>
#endif

/* Declare new lval Struct */
typedef struct lval {
	int type;
	long num;
	/* Errors and symbol types have strings */
	char* err;
	char* sym;
	/* Count and pointer to a list of "lval*" */
	int count;
	struct lval** cell;
} lval;

/* Enumerations of Lval types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

/* Enumerations of Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Create a new number type lval */
lval* lval_num(long x) {
	lval v = malloc(sizeof(lval));
	v.type = LVAL_NUM;
	v.num = x;
	return v;
}

/* Create the error type lval */
lval* lval_err(char* m) {
	lval v = malloc(sizeof(lval));
	v.type = LVAL_ERR;
	v.err = malloc(strlen(m) + 1);
	strcpy(v->err, m);
	return v;
}

lval* lval_sym(char* s) {
	lval* v = malloc(sizeof(lval));
	v->type  =LVAL_SYM;
	v->sym = malloc(strlen(s) + 1);
	strcpy(v->sym, s);
	return v;
}

/* Print an lval */
void lval_print(lval v) {
	switch (v.type) {

		case LVAL_NUM: printf("%li", v.num); break;

		case LVAL_ERR:
			if (v.err == LERR_DIV_ZERO) {
				printf("Error: Division by Zero!");
			}
			if (v.err == LERR_BAD_OP) {
				printf("Error: Invalid Operator.");
			}
			if (v.err == LERR_BAD_NUM) {
				printf("Error: Invalid Number.");
			}
		break;
	}
}

/* print an lval line */
void lval_println(lval v) { lval_print(v); putchar('\n'); }

int number_of_nodes(mpc_ast_t* t) {
	if (t->children_num == 0) { return 1; }
	if (t->children_num >= 1) {
		int total = 1;
		for (int i = 0; i < t->children_num; i++) {
			total = total + number_of_nodes(t->children[i]);
		}
		printf("%i\n", total);
		return total;
	}
	return 0;
}

/* Use operator strings to determine iwhc operation to perform. */
lval eval_op(lval x, char* op, lval y) {

	if (x.type == LVAL_ERR) { return x; }
	if (y.type == LVAL_ERR) { return y; }

	if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
	if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
	if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
	if (strcmp(op, "/") == 0) {
		return y.num == 0
			? lval_err(LERR_DIV_ZERO)
			: lval_num(x.num / y.num);
	}

	return lval_err(LERR_BAD_OP);
}

/* Recursive evaluation */
lval eval(mpc_ast_t* t) {

	/* Return number directory(base case) */
	if (strstr(t->tag, "number")) {
		errno = 0;
		long x = strtol(t->contents, NULL, 10);
		return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
	}

	/* operator is second child */
	char* op = t->children[1]->contents;

	/* third child in x */
	lval x = eval(t->children[2]);

	/* Iterate remaining childedn and combining */
	int i = 3;
	while (strstr(t->children[i]->tag, "expr")) {
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}

	return x;
}

int main(int argc, char** argv) {
  mpc_parser_t* Number    = mpc_new("number");
  mpc_parser_t* Operator  = mpc_new("operator");
  mpc_parser_t* Expr      = mpc_new("expr");
  mpc_parser_t* Lispy     = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                     \
      number        : /-?[0-9]+/ ; \
      operator      : '+' | '-' | '*' | '/'; \
      expr    : <number> | '(' <operator> <expr>+ ')' ; \
      lispy         : /^/ <operator> <expr>+ /$/ ; \
      ",
      Number, Operator, Expr, Lispy);

    puts("Lispy Version 0.0.0.0.1");
  	puts("Press Ctrl+c to Exit\n");

  	/* Neverending loop*/
  	while(1) {
  		char* input = readline("lispy> ");

  		add_history(input);

  		mpc_result_t r;
			if (mpc_parse("<stdin>", input, Lispy, &r)) {
				/* On sucess Print the AST */
				// mpc_ast_print(r.output);
				// mpc_ast_t* a = r.output;
				// number_of_nodes(a);
				lval result = eval(r.output);
				lval_println(result);
				mpc_ast_delete(r.output);
			} else {
				mpc_err_print(r.error);
				mpc_err_delete(r.error);
			}


  		free(input);
  	}
  	return 0;

  mpc_cleanup(4, Number, Operator, Expr, Lispy);
}
