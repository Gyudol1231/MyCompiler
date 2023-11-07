/* =====================================================
* 2020112012 정규용
* input: "input.txt", "input2.txt", "test.txt"
* output: 생성 규칙
* !!<st> 의 optional한 생성 규칙은 인식하지 않습니다.!!
======================================================== */

# define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define ID_LENGHT 12

void block(FILE* cursor);
void st_list(FILE* cursor);
void dcl(FILE* cursor);
void statement(FILE* cursor);
void st(FILE* cursor);
void assignment(FILE* cursor);
void goto_st(FILE* cursor);
void if_st(FILE* cursor);
void write_st(FILE* cursor);
void condition(FILE* cursor);
void exp(FILE* cursor);
void term(FILE* cursor);
void factor(FILE* cursor);

enum tsymbol {
	tnull = -1,
	tdot,		tcomma,		tsemicolon,	tcolon,		tplus,		tminus,
	/* 1		   2		   3		   4		   5		   6  */
	tmul,		tdiv,		tequal,		tassign,		tlparen,	trparen,
	/* 7		   8		   9		   10		   11		   12 */
	tgreat,		tless,		teof,		tnumber,	tid,
	/* 13		   14		   15		   16		   17			  */
	// ...................  word symbols .............................//
	tbegin,		tend,		tgoto,		tif,		tthen,		telse,
	/* 18		   19		   20		   21		   22		   23 */
	tfi,		toutput,	tinput,		tlabel,		tinteger
	/* 24		   25		   26		   27		   28			  */
};
char* tokenName[] = {
	".",		",",		";",		":",		"+",		"-",
	/* 1		   2		   3		   4		   5		   6  */
	"*",		"/",		"=",		"=>",		"(",		")",
	/* 7		   8		   9		   10		   11		   12 */
	"<",		">",		"eof",		"number",	"id",
	/* 13		   14		   15		   16		   17			  */
	// ...................  word symbols .............................//
	/* 18		   19		   20		   21		   22		   23 */
	"begin",	"end",		"goto",		"if",		"then",		"else",
	/* 24		   25		   26		   27		   28			  */
	"fi",		"output",	"input",	"label",	"integer"
};
struct tokenType {
	int type;
	union {
		char id[ID_LENGHT];
		int num;
	} value;
} nextSymbol;


struct tokenType getNextSymbol(FILE* cursor) {
	int i;
	char ch, id[ID_LENGHT];
	struct tokenType token;
	token.type = tnull;

	do {
		while (isspace(ch = fgetc(cursor)));	// skip blanks
		if (isdigit(ch)) {
			i = 0;
			memset(id, '\0', ID_LENGHT);
			do {
				id[i++] = ch;
			} while (isdigit(ch = fgetc(cursor)) && i < ID_LENGHT);
			ungetc(ch, cursor);
			token.type = tnumber;
			token.value.num = atoi(id);
		}
		else if (isalpha(ch)) {
			i = 0;
			memset(id, '\0', ID_LENGHT);
			do {
				id[i++] = ch;
			} while (isalpha(ch = fgetc(cursor)) && i < ID_LENGHT);
			ungetc(ch, cursor);
			if (!strcmp(id, "begin")) {
				token.type = tbegin;
			}
			else if (!strcmp(id, "end")) {
				token.type = tend;
			}
			else if (!strcmp(id, "goto")) {
				token.type = tgoto;
			}
			else if (!strcmp(id, "if")) {
				token.type = tif;
			}
			else if (!strcmp(id, "then")) {
				token.type = tthen;
			}
			else if (!strcmp(id, "else")) {
				token.type = telse;
			}
			else if (!strcmp(id, "fi")) {
				token.type = tfi;
			}
			else if (!strcmp(id, "output")) {
				token.type = toutput;
			}
			else if (!strcmp(id, "integer")) {
				token.type = tinteger;
			}
			else if (!strcmp(id, "label")) {
				token.type = tlabel;
			}
			else {
				token.type = tid;
				strcpy(token.value.id, id);
			}
		}
		else {
			switch (ch) {
			case '(':	token.type = tlparen;			break;
			case ')':	token.type = trparen;			break;
			case ',':	token.type = tcomma;			break;
			case ';':	token.type = tsemicolon;		break;
			case ':':	token.type = tcolon;			break;
			case '.':	token.type = tdot;				break;
			case '+':	token.type = tplus;				break;
			case '-':	token.type = tminus;			break;
			case '*':	token.type = tmul;				break;
			case '/':	token.type = tdiv;				break;
			case '>':	token.type = tgreat;			break;
			case '<':	token.type = tless;				break;
			case '=':
				ch = fgetc(cursor);
				if (ch == '>') token.type = tassign;
				else {
					token.type = tequal;
					ungetc(ch, cursor);
				}
				break;
			case EOF:	token.type = teof;			    break;
			}
		}
	} while (token.type == tnull);

	return token;
}

void spl_program(FILE* cursor) {
	printf("start <spl program>\n");
	if (nextSymbol.type == tbegin) {
		printf("<spl program> expands <block>\n");
		block(cursor, nextSymbol);
		if (nextSymbol.type == tdot) {
			printf("<spl program> generates '.'\n");
			return;
		}
		else {
			printf("error: spl_program\n");
			exit(0);
		}
	}
	else {
		printf("error: spl_program\n");
		exit(0);
	}
}
void block(FILE* cursor) {
	if (nextSymbol.type == tbegin) {
		printf("<block> generates 'begin'\n");
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: block\n");
		exit(0);
	}
	while (nextSymbol.type == tlabel || nextSymbol.type == tinteger) {
		printf("<block> expands <dcl>\n");
		dcl(cursor, nextSymbol);
		if (nextSymbol.type == tsemicolon) {
			printf("<block> generates ';'\n");
			nextSymbol = getNextSymbol(cursor);
		}
		else {
			printf("error: block\n");
			exit(0);
		}
	}
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tgoto ||
		nextSymbol.type == tif ||
		nextSymbol.type == toutput ||
		nextSymbol.type == tbegin ||
		nextSymbol.type == tlparen
		) {
		printf("<block> expands <st_list>\n");
		st_list(cursor, nextSymbol);
	}
	else {
		printf("error: block\n");
		exit(0);
	}
	if (nextSymbol.type == tend) {
		printf("<block> generates 'end'\n");
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: block\n");
		exit(0);
	}
}
void dcl(FILE* cursor) {
	if (nextSymbol.type == tlabel || nextSymbol.type == tinteger) {
		printf("<dcl> generates '%s'\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: dcl\n");
		exit(0);
	}
	if (nextSymbol.type == tid) {
		printf("<dcl> generates '%s'\n", nextSymbol.value.id);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: dcl\n");
		exit(0);
	}
	while (nextSymbol.type == tcomma) {
		printf("<dcl> generates '%s'\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
		if (nextSymbol.type == tid) {
			printf("<dcl> generates '%s'\n", nextSymbol.value.id);
			nextSymbol = getNextSymbol(cursor);
		}
		else {
			printf("error: dcl\n");
			exit(0);
		}
	}
}
void st_list(FILE* cursor) {
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tgoto ||
		nextSymbol.type == tif ||
		nextSymbol.type == toutput ||
		nextSymbol.type == tbegin ||
		nextSymbol.type == tlparen
		) {
		printf("<st_list> expands <st>\n");
		st(cursor, nextSymbol);
	}
	else {
		printf("error: st_list\n");
		exit(0);
	}
	while (nextSymbol.type == tsemicolon) {
		printf("<st_list> generates '%s'\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
		if (nextSymbol.type == tid ||
			nextSymbol.type == tnumber ||
			nextSymbol.type == tinput ||
			nextSymbol.type == tgoto ||
			nextSymbol.type == tif ||
			nextSymbol.type == toutput ||
			nextSymbol.type == tbegin ||
			nextSymbol.type == tlparen
			) {
			printf("<st_list> expands <st>\n");
			st(cursor, nextSymbol);
		}
		else {
			printf("error: st_list\n");
			exit(0);
		}
	}
}
void st(FILE* cursor) {
	// [ <id> ':' ] 추가 해야함
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tgoto ||
		nextSymbol.type == tif ||
		nextSymbol.type == toutput ||
		nextSymbol.type == tbegin ||
		nextSymbol.type == tlparen
		) {
		printf("<st> expands <statement>\n");
		statement(cursor, nextSymbol);
	}
	else {
		printf("error: st\n");
		exit(0);
	}
}
void statement(FILE* cursor) {
	switch (nextSymbol.type) {
	case tgoto:
		printf("<statement> expands <goto>\n");
		goto_st(cursor, nextSymbol);
		break;
	case tif:
		printf("<statement> expands <if_st>\n");
		if_st(cursor, nextSymbol);
		break;
	case toutput:
		printf("<statement> expands <write_st>\n");
		write_st(cursor, nextSymbol);
		break;
	case tbegin:
		printf("<statement> expands <block>\n");
		block(cursor, nextSymbol);
		break;
	case tid:
	case tnumber:
	case tinput:
	case tsemicolon:
		printf("<statement> expands <assignment>\n");
		assignment(cursor, nextSymbol);
		break;
	default:
		printf("error: statement\n");
		exit(0);
		break;
	}
}
void assignment(FILE* cursor) {
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tsemicolon
		) {
		printf("<assignment> expands <exp>\n");
		exp(cursor, nextSymbol);
	}
	else {
		printf("error: assignment\n");
		exit(0);
	}
	if (nextSymbol.type == tassign) {
		printf("<assignment> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: assignment\n");
		exit(0);
	}
	if (nextSymbol.type == tid) {
		printf("<assignment> generates \"%s\"\n", nextSymbol.value.id);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: assignment\n");
		exit(0);
	}
	while (nextSymbol.type == tassign) {
		printf("<assignment> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
		if (nextSymbol.type == tid) {
			printf("<assignment> generates \"%s\"\n", nextSymbol.value.id);
			nextSymbol = getNextSymbol(cursor);
		}
		else {
			printf("error: assignment\n");
			exit(0);
		}
	}
}
void goto_st(FILE* cursor) {
	if (nextSymbol.type == tgoto) {
		printf("<goto_st> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: goto_st\n");
		exit(0);
	}
	if (nextSymbol.type == tid) {
		printf("<goto_st> generates \"%s\"\n", nextSymbol.value.id);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: goto_st\n");
		exit(0);
	}
}
void if_st(FILE* cursor) {
	if (nextSymbol.type == tif) {
		printf("<if_st> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: if_st\n");
		exit(0);
	}
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tsemicolon
		) {
		printf("<if_st> expands <condition>\n");
		condition(cursor);
	}
	else {
		printf("error: if_st\n");
		exit(0);
	}
	if (nextSymbol.type == tthen) {
		printf("<if_st> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: if_st\n");
		exit(0);
	}
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tgoto ||
		nextSymbol.type == tif ||
		nextSymbol.type == toutput ||
		nextSymbol.type == tbegin ||
		nextSymbol.type == tlparen
		) {
		printf("<if_st> expands <st_list>\n");
		st_list(cursor);
	}
	else {
		printf("error: if_st\n");
		exit(0);
	}
	if (nextSymbol.type == telse) {
		printf("<if_st> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
		if (nextSymbol.type == tid ||
			nextSymbol.type == tnumber ||
			nextSymbol.type == tinput ||
			nextSymbol.type == tgoto ||
			nextSymbol.type == tif ||
			nextSymbol.type == toutput ||
			nextSymbol.type == tbegin ||
			nextSymbol.type == tlparen
			) {
			printf("<if_st> expands <st_list>\n");
			st_list(cursor);
		}
		else {
			printf("error: if_st\n");
			exit(0);
		}
	}
	if (nextSymbol.type == tfi) {
		printf("<if_st> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: if_st\n");
		exit(0);
	}
}
void write_st(FILE* cursor) {
	if (nextSymbol.type == toutput) {
		printf("<write_st> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: write_st\n");
		exit(0);
	}
	if (nextSymbol.type == tlparen) {
		printf("<write_st> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: write_st\n");
		exit(0);
	}
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tsemicolon
		) {
		printf("<write_st> expands <exp>\n");
		exp(cursor);
	}
	else {
		printf("error: write_st\n");
		exit(0);
	}
	if (nextSymbol.type == tcomma) {
		printf("<write_st> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
		if (nextSymbol.type == tid ||
			nextSymbol.type == tnumber ||
			nextSymbol.type == tinput ||
			nextSymbol.type == tsemicolon
			) {
			printf("<write_st> expands <exp>\n");
			exp(cursor);
		}
		else {
			printf("error: write_st\n");
			exit(0);
		}
	}
	if (nextSymbol.type == trparen) {
		printf("<write_st> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: write_st\n");
		exit(0);
	}
}
void condition(FILE* cursor) {
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tsemicolon
		) {
		printf("<condition> expands <exp>\n");
		exp(cursor);
	}
	else {
		printf("error: condition\n");
		exit(0);
	}
	if (nextSymbol.type == tless ||
		nextSymbol.type == tgreat ||
		nextSymbol.type == tequal
		) {
		printf("<condition> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
	}
	else {
		printf("error: condition\n");
		exit(0);
	}
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tsemicolon
		) {
		printf("<condition> expands <exp>\n");
		exp(cursor);
	}
	else {
		printf("error: condition\n");
		exit(0);
	}
}
void exp(FILE* cursor) {
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tsemicolon
		) {
		printf("<exp> expands <term>\n");
		term(cursor);
	}
	else {
		printf("error: exp\n");
		exit(0);
	}
	if (nextSymbol.type == tplus ||
		nextSymbol.type == tminus
		) {
		printf("<exp> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
		if (nextSymbol.type == tid ||
			nextSymbol.type == tnumber ||
			nextSymbol.type == tinput ||
			nextSymbol.type == tsemicolon
			) {
			printf("<exp> expands <term>\n");
			term(cursor);
		}
		else {
			printf("error: exp\n");
			exit(0);
		}
	}
}
void term(FILE* cursor) {
	if (nextSymbol.type == tid ||
		nextSymbol.type == tnumber ||
		nextSymbol.type == tinput ||
		nextSymbol.type == tsemicolon
		) {
		printf("<term> expands <factor>\n");
		factor(cursor);
	}
	else {
		printf("error: term\n");
		exit(0);
	}
	if (nextSymbol.type == tmul ||
		nextSymbol.type == tdiv
		) {
		printf("<term> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
		if (nextSymbol.type == tid ||
			nextSymbol.type == tnumber ||
			nextSymbol.type == tinput ||
			nextSymbol.type == tsemicolon
			) {
			printf("<term> expands <factor>\n");
			factor(cursor);
		}
		else {
			printf("error: term\n");
			exit(0);
		}
	}
}
void factor(FILE* cursor) {
	switch (nextSymbol.type) {
	case tinput:
		printf("<factor> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
		break;
	case tid:
		printf("<factor> generates \"%s\"\n", nextSymbol.value.id);
		nextSymbol = getNextSymbol(cursor);
		break;
	case tnumber:
		printf("<factor> generates %d\n", nextSymbol.value.num);
		nextSymbol = getNextSymbol(cursor);
		break;
	case trparen:
		printf("<factor> generates \"%s\"\n", tokenName[nextSymbol.type]);
		nextSymbol = getNextSymbol(cursor);
		if (nextSymbol.type == tid ||
			nextSymbol.type == tnumber ||
			nextSymbol.type == tinput ||
			nextSymbol.type == tsemicolon
			) {
			printf("<factor> expands <exp>\n");
			exp(cursor);
		}
		else {
			printf("error: factor\n");
			exit(0);
		}
		if (nextSymbol.type == trparen) {
			printf("<factor> generates \"%s\"\n", tokenName[nextSymbol.type]);
			nextSymbol = getNextSymbol(cursor);
		}
		else {
			printf("error: factor\n");
			exit(0);
		}
		break;
	default:
		printf("error: factor\n");
		exit(0);
		break;
	}
}

int main(void) {
	FILE* ifs;

	//ifs = fopen("test.txt", "r");
	ifs = fopen("input.txt", "r");
	//ifs = fopen("input2.txt", "r");
	if (ifs == NULL) {
		printf("파일을 열 수 없습니다.\n");
		return 1;
	}

	nextSymbol = getNextSymbol(ifs);
	spl_program(ifs, nextSymbol);

	printf("end :)");

	//while (!feof(ifs) && token.type != tdot) {
	//	token = getNextSymbol(ifs);
	//	printf("<<getNextSymbol>>\n");
	//	printf("terminal: %s\n", tokenName[token.type]);
	//	if (token.type == tid || token.type == tnumber) {
	//		printf("value: %s", token.value);
	//	}
	//	printf("\n\n----------------------------------\n");
	//}
	fclose(ifs);
	return 0;
}