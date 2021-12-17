#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TOLOWER(__c) ((__c >= 'A' && __c <= 'Z') ? __c - ('A' - 'a') : __c)

typedef double decimal;

enum tokentype {
	Null,
	Number,
	Operator,
	Function,
	List
};

enum tokenoperator { // TODO fix names to make them nice :P
	Add,
	Sub,
	Mul,
	Div,
	Mod,
	Exp,
	Sin,
	Cos,
	Tan,
	Factorial,
	Random,
	Range,
	Sum,
	Product,
	START,
	STOP
};

union tokenvalue {
	enum   tokenoperator  OP; // operator
	decimal               NV; // number value
	struct tokenlist     *LP; // list point
	unsigned int          IV; // int value
};

struct token {
	enum tokentype type;
	union tokenvalue value;
};

struct tokenlist {
	unsigned int size;
	struct token* data;
};

unsigned int textlen = 0;
char *text;

static struct timespec _t;
int rand() {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_t);
	return _t.tv_nsec;
}

// static struct tokenliststorage = malloc(1 * sizeof());

void printtoken(struct tokenlist *tokenlist) {
	printf("\x1b[1;96mPrint Token:\n");
	unsigned int maxtokenlen;
	if      (tokenlist->size < 10 ) maxtokenlen = 1;
	else if (tokenlist->size < 100) maxtokenlen = 2;
	else                     maxtokenlen = 3;
	for (unsigned int i = 0; i < tokenlist->size; ++i) {
		printf("\x1b[1;96m%*d:\x1b[0m ", maxtokenlen, i);
		switch (tokenlist->data[i].type) {
			case Null:
				printf("Null\n");
				break;
			case Number:
				printf("Number  : %f\n", tokenlist->data[i].value.NV);
				break;
			case List:
				printf("List    : %u\n", tokenlist->data[i].value.IV);
				break;
			case Operator:
				printf("Operator: ");
				switch (tokenlist->data[i].value.OP) {
					case Add:
						putchar('+');
						break;
					case Sub:
						putchar('-');
						break;
					case Mul:
						putchar('*');
						break;
					case Div:
						putchar('/');
						break;
					case Mod:
						putchar('%');
						break;
					case Exp:
						putchar('^');
						break;
					default:
						putchar('?');
						break;
				}
				putchar('\n');
				break;
			case Function:
				printf("Function: ");
				switch (tokenlist->data[i].value.OP) {
					case Sin:
						putchar('s'); 
						putchar('i'); 
						putchar('n');
						break;
					case Cos:
				 		putchar('c'); 
						putchar('o'); 
						putchar('s');
						break;
					case Tan:
				 		putchar('t'); 
						putchar('a'); 
						putchar('n');
						break;
					case Factorial:
						putchar('f'); 
						putchar('a'); 
						putchar('c');
						putchar('t');
						break;
					case Range:
						putchar('r');
						putchar('a');
						putchar('n');
						putchar('g');
						putchar('e');
						break;
					case Random:
						putchar('r');
						putchar('a');
						putchar('n');
						putchar('d');
						putchar('o');
						putchar('m');
						break;
					case Sum:
						putchar('s');
						putchar('u');
						putchar('m');
						break;
					default:
						putchar('?');
						break;
				}
				putchar('\n');
				break;
			default:
				printf("\x1b[1;31mError:\x1b[0m Unknown type\n");
				break;
		}
	}
}
	static struct tokenlist temptoken;

struct tokenlist tokenize(unsigned int i, unsigned int stop) {

	struct tokenlist token;
	token.data = malloc(1 * sizeof(struct token));
	token.size = 0;

	decimal exponent;
	decimal exponentchange;
	unsigned int m = 0;
	unsigned int bracketcount;
	unsigned int bracketstart;

	// note iterator for token is tokenlen which gets increased as iterates
	while (i < stop) {

		// printf("Char %d: %c\n", i, text[i]);

		// TODO switch statement
		if (text[i] == '+') {
			token.data[token.size].type     = Operator;
			token.data[token.size].value.OP = Add; 
		} else if (text[i] == '-') {
			token.data[token.size].type     = Operator;
			token.data[token.size].value.OP = Sub; 
		} else if (text[i] == '*') {
			token.data[token.size].type     = Operator;
			token.data[token.size].value.OP = Mul; 
		} else if (text[i] == '/') {
			token.data[token.size].type     = Operator;
			token.data[token.size].value.OP = Div; 
		} else if (text[i] == '%') {
			token.data[token.size].type     = Operator;
			token.data[token.size].value.OP = Mod; 
		} else if (text[i] == '^') {
			token.data[token.size].type     = Operator;
			token.data[token.size].value.OP = Exp; 
		} else if ((text[i] == '0')) {
			++i;
			switch (text[i]) {
				case 'b': //byte
					++i;
					exponentchange = 2;
					goto tokenizecontinuenumeration;
				case 'o': //oct
					++i;
					exponentchange = 8;
					goto tokenizecontinuenumeration;
				case 'x': //hex (TODO, characters a-f)
					++i;
					exponentchange = 16;
					goto tokenizecontinuenumeration;
				default:
					--i;
					exponentchange = 10;
					goto tokenizecontinuenumeration;
			}
		} else if ((text[i] >= '1' && text[i] <= '9') || text[i] == '.') { // if digit

			exponentchange = 10;

			tokenizecontinuenumeration:

			token.data[token.size].type     = Number;
			token.data[token.size].value.NV = 0;

			if (text[i] == '.') goto tokenizedecimalplace;
			
			while (
				(i < stop) && ((exponentchange == 16) ? (
					(i < stop && text[i] >= '0' && text[i] <= '9') ||
					(i < stop && text[i] >= 'a' && text[i] <= 'f')
				) : (
					(i < stop && text[i] >= '0' && text[i] <= '0' + exponentchange) 	
				))
			) {
				++i;
				printf("%d %c\n", i, text[i]);
			};// goto back of number to read back to front 
			--i;
			
			exponent = 1;
			m = 0;
			while (
				i < stop && (
					(text[i] >= '0' && text[i] <= '9') ||
					(text[i] >= 'a' && text[i] <= 'f')
				)
			) {
				if (text[i] >= 'a' && text[i] <= 'f') {
					token.data[token.size].value.NV += (text[i] - 'a' + 10) * exponent;
				} else {
					token.data[token.size].value.NV += (text[i] - '0') * exponent;
				}
				--i;
				exponent *= exponentchange;
				++m;
			}

			i += m;

			if (text[i + 1] == '.') { 
				++i;
				tokenizedecimalplace:
				++i;
				
				exponent = 0.1;

				while ((text[i] >= '0' && text[i] <= ('0' + exponentchange - 1)) || text[i] == '.') {
					if (text[i] == '.') {
						printf("\x1b[1;93mWarn:\x1b[0m too many decimal places, ignoring");
						++i;
						continue;
					}
					token.data[token.size].value.NV += (text[i] - '0') * exponent;
					++i;
					exponent *= 0.1;
				};
				--i;
				
			}

		} else if (text[i] == '(') {

			++i;
			bracketcount = 1;
			bracketstart = i;
			while (bracketcount != 0) {
				if (i >= textlen) {
					printf("\x1b[1;93mWarn:\x1b[0m Missing ')', assuming\n");
					++i;
					break;
				}
				if      (text[i] == '(') ++bracketcount;
				else if (text[i] == ')') --bracketcount;
				++i;
			}
			--i;
			temptoken = tokenize(bracketstart, i);
			token.data = realloc(token.data, (token.size + temptoken.size) * sizeof(struct token));
			
			for (m = 0; m < temptoken.size; ++m) {
				token.data[token.size] = temptoken.data[m];
				++token.size;
			}
			--token.size;
			
		} else if (text[i] == ')') {
			printf("\x1b[1;93mWarn:\x1b[0m Too many ')', ignoring\n");
		} else if (text[i] == '[') {
			
			++i;
			bracketcount = 1;
			bracketstart = i;
			while (bracketcount != 0) {
				if (i >= textlen) {
					printf("\x1b[1;93mWarn:\x1b[0m Missing ']', assuming\n");
					++i;
					break;
				}
				if      (text[i] == '[') ++bracketcount;
				else if (text[i] == ']') --bracketcount;
				++i;
			}

			temptoken = tokenize(bracketstart, i - 1);

			token.data[token.size].type     = List;
			token.data[token.size].value.IV = temptoken.size;

			++token.size;

			token.data = realloc(token.data, (token.size + temptoken.size) * sizeof(struct token));
			
			for (m = 0; m < temptoken.size; ++m) {
				token.data[token.size] = temptoken.data[m];
				++token.size;
			}
			--token.size;
			
		} else if (text[i] == ']') {
			printf("\x1b[1;93mWarn:\x1b[0m Too many ']', ignoring\n");	
		} else if (text[i] == 's') {
			i++;
			if        (text[i] == 'u') {
				i++;
				if (text[i] != 'm' || i > stop) goto tokenizeinvalidsymbol;
				token.data[token.size].value.OP = Sum;
			} else if (text[i] == 'i') {
				i++;
				if (text[i] != 'n' || i > stop) goto tokenizeinvalidsymbol;
				token.data[token.size].value.OP = Sin;
			} else goto tokenizeinvalidsymbol;
			token.data[token.size].type = Function;
		} else if (text[i] == 'c') {
			i++;
			if (text[i] != 'o' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 's' || i > stop) goto tokenizeinvalidsymbol;
			token.data[token.size].type     = Function;
			token.data[token.size].value.OP = Cos;
		} else if (text[i] == 't') {
			i++;
			if (text[i] != 'a' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'n' || i > stop) goto tokenizeinvalidsymbol;
			token.data[token.size].type     = Function;
			token.data[token.size].value.OP = Tan;
		} else if (text[i] == 'f') {
			i++;
			if (text[i] != 'a' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'c' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 't' || i > stop) goto tokenizeinvalidsymbol;
			token.data[token.size].type     = Function;
			token.data[token.size].value.OP = Factorial;
		} else if (text[i] == '!') {
			token.data[token.size].type     = Function;
			token.data[token.size].value.OP = Factorial;
		} else if (text[i] == 'r') {
			i++;
			if (text[i] != 'a' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'n' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (i > stop) goto tokenizeinvalidsymbol;
			if (text[i] == 'g') {
				i++;
				if (text[i] != 'e' || i > stop) goto tokenizeinvalidsymbol;
				token.data[token.size].type     = Function;
				token.data[token.size].value.OP = Range;
			} else {
				i--;
				token.data[token.size].type     = Function;
				token.data[token.size].value.OP = Random;
			}
		} else if (text[i] == 'p') {
			i++;
			if (text[i] != 'r' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'o' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'd' || i > stop) goto tokenizeinvalidsymbol;
			token.data[token.size].type     = Function;
			token.data[token.size].value.OP = Product;
		} else if (text[i] == ',') {
			// a lazy way to make lists lmao
			++i;
			continue;
		} else { tokenizeinvalidsymbol:
			printf("\x1b[1;93mWarn:\x1b[0m Invalid symbol '%c', ignoring\n", text[i]);
			++i;
			continue;
		}

		++token.size;
		token.data = realloc(token.data, (token.size + 1) * sizeof(struct token));
		++i;
		
	}

	printtoken(&token);
	// Time to calculate (with bodmas) functions, ^, %, *, /, +, -

	//find sum (functions with list arg)
	for (i = 0; i < token.size; ++i) { 
		if (token.data[i].type != Function) continue;
		if (i == token.size || token.data[i + 1].type != List) continue;
		token.data[i].type = Number;
		bracketcount = token.data[i + 1].value.IV; // bracketcount = list size
		switch (token.data[i].value.OP) {
			case Sum:
				token.data[i].value.NV = 0; 
				for (m = 1; m < bracketcount + 2; ++m) {
					++i;
					token.data[i - m].value.NV += token.data[i].value.NV;
				}
				break;
			case Product:
				token.data[i].value.NV = 1; 
				for (m = 0; m < bracketcount + 2; ++m) {
					token.data[i - m].value.NV *= token.data[i].value.NV;
					++i;
				}
				break;
			default:
				printf("\x1b[1;31mError:\x1b[0m Unknown function (code is broken)\n");
		}

		for (m = i + 1; m < token.size; ++m) {
			token.data[m - bracketcount - 1] = token.data[m];
		}
		token.size -= bracketcount + 1;
		i          -= bracketcount + 1;
		token.data = realloc(token.data, token.size * sizeof(struct token));
		printtoken(&token);

	}
	
	//find sin, cos, tan (functions)
	for (i = 0; i < token.size; ++i) { 
		if (token.data[i].type != Function) continue;
		if (i == token.size || token.data[i + 1].type != Number) continue;
		switch (token.data[i].value.OP) {
			case Sin:
				token.data[i].value.NV = sin(token.data[i + 1].value.NV);
				break;
			case Cos:
				token.data[i].value.NV = cos(token.data[i + 1].value.NV);
				break;
			case Tan:
				token.data[i].value.NV = tan(token.data[i + 1].value.NV);
				break;
			case Factorial:
				token.data[i].value.NV = 1;
				for (m = 2; m <= token.data[i + 1].value.NV; ++m) {
					token.data[i].value.NV *= m;
				}
				break;
			case Random:
				// handle 2 args
				if (i == token.size - 1 || token.data[i + 2].type != Number) goto tokenizetypeerror;
				// make sure we do random range: min -> max
				if (token.data[i + 1].value.NV > token.data[i + 2].value.NV) {
					token.data[i].value.NV = fmod((decimal)rand(), (
						token.data[i + 1].value.NV - token.data[i + 2].value.NV
					) + token.data[i + 2].value.NV - 1) + 2;
				} else {
					token.data[i].value.NV = fmod((decimal)rand(), (
						token.data[i + 2].value.NV - token.data[i + 1].value.NV
					) + token.data[i + 1].value.NV - 1) + 2;
				}
				// TODO clean this mess up
				token.data[i].type = Number;	
				for (m = i + 1; m < token.size; ++m) {
					token.data[m] = token.data[m + 1];
				}
				token.data = realloc(token.data, token.size * sizeof(struct token));
				--token.size;
				// TODO
				break;
			case Range:
				// handle 2 args
				if (i == token.size - 1 || token.data[i + 2].type != Number) goto tokenizetypeerror;
				// swap around args if 2 > 1
				if (token.data[i + 2].value.NV < token.data[i + 1].value.NV) { 
					m = token.data[i + 1].value.NV;
					token.data[i + 1].value.NV = token.data[i + 2].value.NV;
					token.data[i + 2].value.NV = m;
				}
				token.data[i].type = List;
				token.data[i].value.IV = (unsigned int)token.data[i + 2].value.NV - (unsigned int)token.data[i + 1].value.NV + 1;

				token.size = token.size + token.data[i].value.IV - 2;
				token.data = realloc(token.data, (token.size + 1) * sizeof(struct token));

				for (m = token.size - 1; m > i + token.data[i].value.IV; --m) {
					token.data[m] = token.data[m - token.data[i].value.IV + 2];
				}

				for (m = 2; m <= token.data[i].value.IV; ++m) {
					token.data[i + m].type = Number;
					token.data[i + m].value.NV = m + token.data[i + 1].value.NV - 1;
				}

				i += token.data[i].value.IV;
				continue;	
			
			default:
				printf("\x1b[1;31mError:\x1b[0m Unknown function (code is broken)\n");
		}
		token.data[i].type = Number;
		for (m = i + 1; m < token.size; ++m) {
			token.data[m] = token.data[m + 1];
		}

		token.data = realloc(token.data, token.size * sizeof(struct token));
		--token.size;
		--i;
	}

	//find ^
	for (i = 0; i < token.size; ++i) {
		
		if (token.data[i].type != Operator) continue;

		// only have to do this check once, as it applies the same to every operator
		if (
			i == 0          || 
			i == token.size ||
			token.data[i - 1].type != Number || 
			token.data[i + 1].type != Number
		) goto tokenizetypeerror;

		if (token.data[i].value.OP != Exp) continue;
				
		token.data[i - 1].value.NV = pow(token.data[i - 1].value.NV, token.data[i + 1].value.NV);
		
		for (m = i; m < token.size; ++m) {
			token.data[m] = token.data[m + 2];
		}
		token.size -= 2;
		token.data = realloc(token.data, (token.size + 1) * sizeof(struct token));
		i -= 2;
		
	}

	//find %
	for (i = 0; i < token.size; ++i) {
	
		if (
			token.data[i].type     != Operator ||
			token.data[i].value.NV != Exp
		) continue;
				
		token.data[i - 1].value.NV = pow(token.data[i - 1].value.NV, token.data[i + 1].value.NV);
		
		for (m = i; m < token.size; ++m) {
			token.data[m] = token.data[m + 2];
		}
		token.size -= 2;
		token.data = realloc(token.data, (token.size + 1) * sizeof(struct token));
		i -= 2;
		
	}

	//find * /
	for (i = 0; i < token.size; ++i) {

		if (token.data[i].type != Operator) continue;
		
		if        (token.data[i].value.OP == Mul) {
			token.data[i - 1].value.NV = token.data[i - 1].value.NV * token.data[i + 1].value.NV;
		} else if (token.data[i].value.OP == Div) {
			token.data[i - 1].value.NV = token.data[i - 1].value.NV / token.data[i + 1].value.NV;
		} else continue;

		for (m = i; m < token.size; ++m) {
			token.data[m] = token.data[m + 2];
		}
		token.size -= 2;
		token.data = realloc(token.data, (token.size + 1) * sizeof(struct token));
		i -= 2;
		
	}

	//find + -
	for (i = 0; i < token.size; ++i) {
	
		if (token.data[i].type != Operator) continue;
				
		if        (token.data[i].value.OP == Add) {
			token.data[i - 1].value.NV = token.data[i - 1].value.NV + token.data[i + 1].value.NV;
		} else if (token.data[i].value.OP == Sub) {
			token.data[i - 1].value.NV = token.data[i - 1].value.NV - token.data[i + 1].value.NV;
		} else continue;
		
		for (m = i; m < token.size; ++m) {
			token.data[m] = token.data[m + 2];
		}
		token.size -= 2;
		token.data = realloc(token.data, (token.size + 1) * sizeof(struct token));
		i -= 2;
		
	}


	return token;

	tokenizetypeerror:
		printf("\x1b[1;31mError:\x1b[0m Expected number\n"); 
		return token;

}

int main(int argc, char *argv[]) {

	unsigned int j = 0;
	unsigned int m = 0;

	if (argc <= 1) {
		
		char *temptext = malloc(100 * sizeof(char));

		printf("Calculation: ");
		fgets(temptext, 100, stdin);
		
		for (unsigned int i = 0; temptext[i] != '\n'; ++i) textlen++; // get size of string
		text = malloc(textlen * sizeof(char));

		// m is iterator for text (read m = 0)
		for (unsigned int i = 0; i < textlen; ++i) {
			if (temptext[i] != ' ' && temptext[i] != '\\') {
				text[m] = TOLOWER(temptext[i]);
				++m;
			}
		}
		free(temptext);

		goto tokenize;
	}

	// get size of args
	for (int i = 1; i < argc; ++i) {

		while (argv[i][m] != '\0') {
			if (argv[i][m] != ' ' && argv[i][m] != '\\') {
				++textlen;
			}
			++m;
		}

		m = 0;
	}

	// put all args into text var
	text = malloc(textlen * sizeof(char));
	for (int i = 1; i < argc; ++i) {
	
		while (argv[i][m] != '\0') {
			if (argv[i][m] != ' ' && argv[i][m] != '\\') {

				text[j] = TOLOWER(argv[i][m]);
				++j;

			}
			++m;
		}

		m = 0;
	}
	
	tokenize:

		clock_t start;

		start = clock();
		decimal result =  tokenize(0, textlen).data[0].value.NV;
		
		printf("\x1b[1;96mResult:\x1b[0m %f\n\x1b[1;96mTime  :\x1b[0m %fs\n", 
			result,
			((double) (clock() - start)) / CLOCKS_PER_SEC
		);
	
	return 0;
	
}
