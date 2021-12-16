#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum tokentype {
	Null,
	Number,
	Operator,
	Function
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
	Factorial
};

struct token {
	enum tokentype type;
	float value; // this isnt the actuall value, typecast to float or enum tokentype
};

unsigned int textlen = 0;
char *text;

void printtoken(struct token * *token, unsigned int tokenlen) {
	unsigned int maxtokenlen;
	if      (tokenlen < 10 ) maxtokenlen = 1;
	else if (tokenlen < 100) maxtokenlen = 2;
	else                     maxtokenlen = 3;
	for (unsigned int i = 0; i < tokenlen; ++i) {
		printf("Token %*d: ", maxtokenlen, i);
		if ((*token)[i].type == Null) {
			printf("Null\n");
		} else if ((*token)[i].type == Number) {
			printf("Number: %f\n", (float)(*token)[i].value);
		} else {
			if ((*token)[i].type == Operator) {
				printf("Operator: ");
			} else {
				printf("Function: ");
			}
			switch ((int)(*token)[i].value) { // (int) to make gcc happy
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
				default:
					putchar('?');
					break;
			}
			putchar('\n');
		}
	}
}

float tokenize(unsigned int i, unsigned int stop) {

	unsigned int tokenlen = 0;
	struct token *token = token = malloc(1 * sizeof(struct token));

	float exponent;
	unsigned int m = 0;
	unsigned int bracketcount;
	unsigned int bracketstart;

	// note iterator for token is tokenlen which gets increased as iterates
	while (i < stop) {

		if (text[i] == '+') {
			token[tokenlen].type  = Operator;
			token[tokenlen].value = Add; 
		} else if (text[i] == '-') {
			token[tokenlen].type  = Operator;
			token[tokenlen].value = Min; 
		} else if (text[i] == '*') {
			token[tokenlen].type  = Operator;
			token[tokenlen].value = Mul; 
		} else if (text[i] == '/') {
			token[tokenlen].type  = Operator;
			token[tokenlen].value = Div; 
		} else if (text[i] == '%') {
			token[tokenlen].type  = Operator;
			token[tokenlen].value = Mod; 
		} else if (text[i] == '^') {
			token[tokenlen].type  = Operator;
			token[tokenlen].value = Exp; 
		} else if ((text[i] >= '0' && text[i] <= '9') || text[i] == '.') { // if digit


			token[tokenlen].type  = Number;
			token[tokenlen].value = 0;

			if (text[i] == '.') goto tokenizedecimalplace;
			
			while (i < stop && text[i] >= '0' && text[i] <= '9') ++i;// goto back of number to read back to front 
			--i;
			
			exponent = 1;
			while (i < stop && text[i] >= '0' && text[i] <= '9') {
				token[tokenlen].value += (text[i] - '0') * exponent;
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
					token[tokenlen].value = (float)(token[tokenlen].value) + (float)((text[i] - '0') * exponent);
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
			token[tokenlen].type  = Number;
			token[tokenlen].value = tokenize(bracketstart, i);
			
		} else if (text[i] == ')') {
			printf("Warn: Too many ')', ignoring\n");
		} else if (text[i] == 's') {
			i++;
			if (text[i] != 'i' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'n' || i > stop) goto tokenizeinvalidsymbol;
			token[tokenlen].type  = Function;
			token[tokenlen].value = Sin;
		} else if (text[i] == 'c') {
			i++;
			if (text[i] != 'o' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 's' || i > stop) goto tokenizeinvalidsymbol;
			token[tokenlen].type  = Function;
			token[tokenlen].value = Cos;
		} else if (text[i] == 't') {
			i++;
			if (text[i] != 'a' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'n' || i > stop) goto tokenizeinvalidsymbol;
			token[tokenlen].type  = Function;
			token[tokenlen].value = Tan;
		} else if (text[i] == 'f') {
			i++;
			if (text[i] != 'a' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 'c' || i > stop) goto tokenizeinvalidsymbol;
			i++;
			if (text[i] != 't' || i > stop) goto tokenizeinvalidsymbol;
			token[tokenlen].type  = Function;
			token[tokenlen].value = Factorial;
		} else if (text[i] == '!') {
			token[tokenlen].type  = Function;
			token[tokenlen].value = Factorial;
		} else {
			tokenizeinvalidsymbol:
				printf("Warn: Invalid symbol, ignoring");
		}
		
		++tokenlen;
		token = realloc(token, (1 + tokenlen) * sizeof(struct token));
		++i;
		
	}
	
	// Time to calculate (with bodmas) ^, %, *, /, +, -

	//find sin, cos, tan (functions)
	for (i = 0; i < tokenlen; ++i) {
		if (token[i].type != Function) continue;
		if (i == tokenlen) {
			printf("Error: Expected number not nothing \n"); return 0;
		}
		if (token[i + 1].type != Number) {
			printf("Error: Expected number not operator\n"); return 0;	
		}
		switch ((int)token[i].value) {// (int) to make gcc happy
			case Sin:
				token[i].value = sin(token[i + 1].value);
				break;
			case Cos:
				token[i].value = cos(token[i + 1].value);
				break;
			case Tan:
				token[i].value = tan(token[i + 1].value);
				break;
			case Factorial:
				token[i].value = 1;
				for (m = 1; m < token[i + 1].value; ++m) {
					token[i].value *= m;
				}
				break;
			default:
				printf("Error: Unknown function (code is broken)");
		}	
		token[i].type = Number;	
		for (m = i + 1; m < tokenlen; ++m) { //todo convert x++ ++X TODO
			token[m] = token[m + 1];
		}
		tokenlen -= 1;
		i -= 1;
		token = realloc(token, (tokenlen + 1) * sizeof(struct token));
		printtoken(&token, tokenlen);
	}

	//find ^
	for (i = 0; i < tokenlen; ++i) {
		if (token[i].type != Operator) continue;
		if (token[i].value != Exp) continue;
		if (i == 0 || i == tokenlen) {
			printf("Error: Expected number not nothing \n"); return 0;
		}
		if (token[i - 1].type != Number || token[i + 1].type != Number) {
			printf("Error: Expected number not operator\n"); return 0;	
		}
		token[i - 1].value = pow(token[i - 1].value, token[i + 1].value);
		for (m = i; m < tokenlen; ++m) { //todo convert x++ ++X TODO
			token[m] = token[m + 2];
		}
		tokenlen -= 2;
		i -= 2;
		token = realloc(token, (tokenlen + 1) * sizeof(struct token));
	}

	//find %
	for (i = 0; i < tokenlen; ++i) {
		if (token[i].type != Operator) continue;
		if (token[i].value != Mod) continue;
		if (i == 0 || i == tokenlen) {
			printf("Error: Expected number not nothing \n"); return 0;
		}
		if (token[i - 1].type != Number || token[i + 1].type != Number) {
			printf("Error: Expected number not operator\n"); return 0;	
		}
		token[i - 1].value = fmod(token[i - 1].value, token[i + 1].value);
		for (m = i; m < tokenlen; ++m) { //todo convert x++ ++X TODO
			token[m] = token[m + 2];
		}
		tokenlen -= 2;
		i -= 2;
		token = realloc(token, (tokenlen + 1) * sizeof(struct token));
	}

	//find * /
	for (i = 0; i < tokenlen; ++i) {
		if (token[i].type != Operator) continue;
		if (token[i].value != Mul && token[i].value != Div) continue;
		if (i == 0 || i == tokenlen) {
			printf("Error: Expected number not nothing \n"); return 0;
		}
		if (token[i - 1].type != Number || token[i + 1].type != Number) {
			printf("Error: Expected number not operator\n"); return 0;	
		}
		if (token[i].value == Mul)
			token[i - 1].value = token[i - 1].value * token[i + 1].value;
		else
			token[i - 1].value = token[i - 1].value / token[i + 1].value;
		for (m = i; m < tokenlen; ++m) { //todo convert x++ ++X TODO
			token[m] = token[m + 2];
		}
		tokenlen -= 2;
		i -= 2;
		token = realloc(token, (tokenlen + 1) * sizeof(struct token));
	}

	//find + -
	for (i = 0; i < tokenlen; ++i) {
		if (token[i].type != Operator) continue;
		if (token[i].value != Add && token[i].value != Min) continue;
		if (i == 0 || i == tokenlen) {
			printf("Error: Expected number not nothing \n"); return 0;
		}
		if (token[i - 1].type != Number || token[i + 1].type != Number) {
			printf("Error: Expected number not operator\n"); return 0;	
		}
		if (token[i].value == Add)
			token[i - 1].value = token[i - 1].value + token[i + 1].value;
		else
			token[i - 1].value = token[i - 1].value - token[i + 1].value;
		for (m = i; m < tokenlen; ++m) { //todo convert x++ ++X TODO
			token[m] = token[m + 2];
		}
		tokenlen -= 2;
		i -= 2;
		token = realloc(token, (tokenlen + 1) * sizeof(struct token));
	}

	//printtoken(&token, tokenlen);
	free(token);
	return token[0].value;

}

int main(int argc, char *argv[]) {

	if (argc <= 1) {
		printf("Calculation: ");
		scanf("%m[^\n]", &text);
		goto tokenize;
		//printf("Error: No argument!\n");
	}

	// get size of args
	unsigned int m = 0;
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
	unsigned int j = 0;
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
	text[textlen] = '\0';

	// tokenize time :P
	tokenize:

	printf("Result: %f\n", tokenize(0, textlen));

	// print tokens

	return 0;
	
}
