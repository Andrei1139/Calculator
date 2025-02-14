#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAXLEN 100
#ifdef _WIN64
#define clear() system("clear")
#else
#define clear() system("cls")
#endif

enum Token_Type {
    NUMBER = 0, OPERATOR = 1, PARENTHESIS = 2, END = -1
};

typedef struct {
    enum Token_Type type;
    int number;
    char symbol;
} Token;

typedef struct {
    Token tokens[MAXLEN];
    size_t len, index;
} Token_List;

int is_operator(char c) {
    return c == '+' || c == '-' || c == '/' || c == '*';
}

int calculate(Token_List*);

/* Convert string with operations into token list */
Token_List get_expression(const char *string) {
    Token_List list;
    Token token;
    list.len = list.index = 0;
    int nr_open_parentheses = 0, i = 0;

    while (string[i] != '\0' && string[i] != '\n') {
        token.number = 0;
        
        while (string[i] == ' ') {
            ++i;
        }

        if (is_operator(string[i])) {
            if (list.len == 0 || list.tokens[list.len - 1].type == OPERATOR) {
                fprintf(stderr, "Invalid operator position\n");
                exit(-1);
            }
            token.symbol = string[i];
            token.type = OPERATOR;
            list.tokens[list.len++] = token;
            ++i;
        } else if (isdigit(string[i])) {
            while (isdigit(string[i])) {
                token.number = token.number * 10 + string[i] - '0';
                ++i;
            }
            token.type = NUMBER;
            list.tokens[list.len++] = token;
        } else if (string[i] == '(') {
            if (list.tokens[list.len - 1].type == NUMBER) {
                fprintf(stderr, "Coefficients not implemented yet\n");
                exit(-1);
            }
            ++nr_open_parentheses;
            token.symbol = '(';
            token.type = PARENTHESIS;
            list.tokens[list.len++] = token;
            ++i;
        } else if (string[i] == ')') {
            if (nr_open_parentheses == 0) {
                fprintf(stderr, "Invalid parentheses\n");
                exit(-1);
            }
            --nr_open_parentheses;
            token.symbol = ')';
            token.type = PARENTHESIS;
            list.tokens[list.len++] = token;
            ++i;
        } else {
            fprintf(stderr, "Invalid character\n"); // TODO: Implement fractional numbers functionality
            exit(-1);
        }
    }

    if (nr_open_parentheses != 0) {
        fprintf(stderr, "Invalid parentheses\n");
        exit(-1);
    }

    list.tokens[list.len++].type = END;
    return list;
}

/* Return number or result of operations performed within parenthesis pair */
int get_number_or_parentheses(Token_List *list) {
    if (list->tokens[list->index].type == PARENTHESIS) {
        ++list->index;
        return calculate(list);
    }
    return list->tokens[list->index++].number;
}

/* Return number or result of the next multiplications and divisions next to one another */
int get_number_or_product(Token_List *list) {
    int product = get_number_or_parentheses(list);

    while (list->tokens[list->index].type != END) {
        if (list->tokens[list->index].symbol == '*') {
            ++list->index;
            product *= get_number_or_parentheses(list);
        } else if (list->tokens[list->index].symbol == '/') {
            ++list->index;
            product /= get_number_or_parentheses(list);
        } else {
            break;
        }
    }

    return product;
}

/* Process each token of the expression using PEMDAS, either for the whole list or just a parenthesis pair */
int calculate(Token_List *list) {
    int result = get_number_or_product(list);

    while (list->tokens[list->index].type != END) {
        if (list->tokens[list->index].symbol == '+') {
            ++list->index;
            result += get_number_or_product(list);
        } else if (list->tokens[list->index].symbol == '-') {
            ++list->index;
            result -= get_number_or_product(list);
        } else {
            ++list->index;
            break;
        }
    }
    return result;
}

int main() {
    clear();

    char line[MAXLEN];
    fgets(line, MAXLEN, stdin);

    Token_List expression = get_expression(line);
    printf("%d\n", calculate(&expression));
}