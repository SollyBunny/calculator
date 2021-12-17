#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


enum tokentype {
	Null,
	Number,
	Operator,
	Function,
	List
};

enum tokenoperator {
	Add,
	Min,
	Mul,
	Div,
	Mod,
	Exp,
	Sin,
	Cos,
	Tan,
	Fac,
	Ran,
	Sum
};

union tokenvalue {
	enum   tokenoperator  OP; // operator
	float                 NV; // number float
	struct tokenlist     *LP; // list point
	int                  _INT;
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

struct timespec _t;
int rand() {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_t);
	return _t.tv_nsec;
}

void printtoken(struct tokenlist *tokenlist) {
	unsigned int maxtokenlen;
	if      (tokenlist->size < 10 ) maxtokenlen = 1;
	else if (tokenlist->size < 100) maxtokenlen = 2;
	else                     maxtokenlen = 3;
	for (unsigned int i = 0; i < tokenlist->size; ++i) {
		printf("Token %*d: ", maxtokenlen, i);
		switch (tokenlist->data[i].type) {
			case Null:
				printf("Null\n");
				break;
			case Number:
				printf("Number  : %f\n", tokenlist->data[i].value.NV);
				break;
			case List:
				printf("List    : %d\n", tokenlist->data[i].value._INT);
				printtoken(tokenlist->data[i].value.LP);
				break;
			case Operator:
				printf("Operator: ");
				switch (tokenlist->data[i].value.OP) {
					case Add:
						putchar('+');
						break;
					case Min:
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
					case Fac:
						putchar('f'); 
						putchar('a'); 
						putchar('c');
						putchar('t');
						break;
					case Ran:
						putchar('r');
						putchar('a');
						putchar('n');
						break;
					case Sum:
						putchar('r');
						putchar('a');
						putchar('n');
						break;
					default:
						putchar('?');
						break;
				}
				putchar('\n');
				break;
			default:
				printf("UNKOWN TYPE\n");
				break;
		}
	}
}

struct tokenlist tokenize(unsigned int i, unsigned int stop) {

	struct tokenlist temptoken;
	struct tokenlist token;
	token.data = malloc(1 * sizeof(struct token));
	token.size = 0;

	float exponent;
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
			token.data[token.size].value.OP = Min; 
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
		} else if ((text[i] >= '0' && text[i] <= '9') || text[i] == '.') { // if digit

			token.data[token.size].type     = Number;
			token.data[token.size].value.NV = 0;

			if (text[i] == '.') goto tokenizedecimalplace;
			
			while (i < stop && text[i] >= '0' && text[i] <= '9') ++i;// goto back of number to read back to front 
			--i;
			
			exponent = 1;
			while (i < stop && text[i] >= '0' && text[i] <= '9') {
				token.data[token.size].value.NV += (text[i] - '0') * exponent;
				--i;
				exponent *= 10;
			}

			while ((exponent /= 10) != 1) ++i;
			++i;

			if (text[i + 1] == '.') { 
				++i;
				tokenizedecimalplace:
				++i;
				
				exponent = 0.1;

				while ((text[i] >= '0' && text[i] <= '9') || text[i] == '.') {
					if (text[i] == '.') {
						printf("Warn: too many decimal places, ignoring");
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
					printf("Warn: Missing ')', assuming\n");
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
			printf("Warn: Too many ')', ignoring\n");
		} else if (text[i] == '[') {

			++i;
			bracketcount = 1;
			bracketstart = i;
			while (bracketcount != 0) {
				if (i >= textlen) {
					printf("Warn: Missing ']', assuming\n");
					++i;
					break;
				}
				if      (text[i] == '[') ++bracketcount;
				else if (text[i] == ']') --bracketcount;
				++i;
			}
			--i;
			temptoken = tokenize(bracketstart, i);
			token.data[token.size].type     = List;
			token.data[token.size].value.LP = &temptoken;

		} else if (text[i] == ']') {
			printf("Warn: Too many ']', ignoring\n");	
		} else if (text[i] == 's') {
			i++;
			if (text[i] != 'i' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'n' || i > stop) goto tokenizeinvalidsymbol;
			token.data[token.size].type     = Function;
			token.data[token.size].value.OP = Sin;
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
			token.data[token.size].value.OP = Fac;
		} else if (text[i] == '!') {
			token.data[token.size].type     = Function;
			token.data[token.size].value.OP = Fac;
		} else if (text[i] == 'r') {
			i++;
			if (text[i] != 'a' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'n' || i > stop) goto tokenizeinvalidsymbol;
			token.data[token.size].type     = Function;
			token.data[token.size].value.OP = Ran;
		} else if (text[i] == ',') {
			// a lazy way to make lists lmao
			++i;
			continue;
		} else { tokenizeinvalidsymbol:
			printf("Warn: Invalid symbol, ignoring\n");
			++i;
			continue;
		}
		
		++token.size;
		token.data = realloc(token.data, (token.size + 1) * sizeof(struct token));
		++i;
		
	}
	
	// Time to calculate (with bodmas) functions, ^, %, *, /, +, -
	printtoken(&token);

	// REMEMBER SUM NEEDS DIF IMPLEMENTATION DUE TO LIST ARG

	//find sin, cos, tan (functions)
	for (i = 0; i < token.size; ++i) { 
		if (token.data[i].type != Function) continue;
		if (i == token.size || token.data[i + 1].type != Number) goto tokenizetypeerror;
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
			case Fac:
				token.data[i].value.NV = 1;
				for (m = 1; m < token.data[i + 1].value.NV; ++m) {
					token.data[i].value.NV *= m;
				}
				break;
			case Ran:
				// handle 2 args
				if (i == token.size - 1 || token.data[i + 2].type != Number) goto tokenizetypeerror;
				// make sure we do random range: min -> max
				if (token.data[i + 1].value.NV > token.data[i + 2].value.NV) {
					token.data[i].value.NV = fmod((float)rand(), (
						token.data[i + 1].value.NV - token.data[i + 2].value.NV + 1
					) + token.data[i + 2].value.NV);
				} else {
					token.data[i].value.NV = fmod((float)rand(), (
						token.data[i + 2].value.NV - token.data[i + 1].value.NV + 1
					) + token.data[i + 1].value.NV);
				}
				break;
			default:
				printf("Error: Unknown function (code is broken)");
		}
		token.data[i].type = Number;	
		for (m = i + 1; m < token.size; ++m) { //todo convert x++ ++X TODO
			token.data[m] = token.data[m + 1];
		}
		--token.size;
		token.data = realloc(token.data, (token.size + 1) * sizeof(struct token));
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
		} else if (token.data[i].value.OP == Min) {
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
		printf("Error: Expected number\n"); 
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
				text[m] = temptext[i];
				++m;
			}
		}
		free(temptext);

		goto tokenize;
		//printf("Error: No argument!\n");
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

				text[j] = argv[i][m];
				++j;

			}
			++m;
		}

		m = 0;
	}
	
	tokenize:
	text[textlen] = '\0';
	printf("'%s'\n", text);

	// tokenize time :P
	

	printf("Result: %f\n", tokenize(0, textlen).data[0].value.NV);

	// print tokens

	return 0;
	
}
