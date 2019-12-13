#ifndef TOKENiZIER
#define TOKENiZIER

#include "tree.h"

struct for_funcs {
    node* *names;
    int amount;

    explicit for_funcs (int size = 5);
    void append (node* element);
    void see ();

    int search_func (node* name);
    ~for_funcs ();
};

for_funcs::for_funcs (int size) {
    names = new node*[size];
    amount = 0;
}

void for_funcs::append (node* element) {
    names[amount] = element;
    ++amount;
}

void for_funcs::see () {
    for (int i = 0; i < amount; ++i) {
        printf ("func: %s\n", names[i]->right->data);
    }
}

int for_funcs::search_func (node* needed) {
    for (int i = 0; i < amount; ++i) {
        if (strcmp (names[i]->data, needed->data) == 0) {
            return i;
        }
    }
    return -1;
}

for_funcs::~for_funcs() {

}

for_funcs* functions = new for_funcs;

bool not_special (char symbol) {
    return (symbol != ' ' && symbol != '\n' && symbol != '.' &&
            symbol != ',' && symbol != '\t' && symbol != '$' &&
            symbol != '+' && symbol != '-' && symbol != '*' &&
            symbol != '/');
}

bool not_divider (char symbol) {
    return (symbol != ' ' && symbol != '\n' &&
            symbol != '\t' && symbol != '$' &&
            symbol != '\0');
}

#define now_parse_that_long_long_name \
    nodes[n_nodes].type = ID;\
    char* id = new char[default_size];\
    char* cur = id;\
        \
    while (not_divider (*str)) {\
        *cur = *str;\
        ++cur;\
        ++str;\
    }\
    if (*(cur - 1) == ',') {\
        *(cur - 1) = '\0';\
    }\
    \
    if (cur - id > default_size) {\
        err_info ("Overwritten id - tokenize\n");\
    }\
    \
    strncpy (data, id, cur - id);\
    //ids->append (id);


struct cell {
    char* name;
    int type;

    explicit cell ();
};

cell::cell() {
    name = new char[default_size];
    type = UNKNOWN;
}

cell* tokenize (char* str, const int max_nodes_number = 100) {

    cell *nodes = new cell[max_nodes_number];
    int n_nodes = 0;

    while (*str != '\0') {

        //printf ("%s\n\n<------------------------------------>\n\n", str);

        while (*str == ' ' || *str == '\n' || *str == '\t' || *str == ',') {
            ++str;
        }

        if (*str == '$') {                                      //comments
            ++str;
            int scanfed = 0;
            char* for_rubbish = new char[default_size];
            sscanf (str, "%[^$]%n", for_rubbish, &scanfed);

            str += scanfed + 1;                                 // + '$'
            delete[] for_rubbish;
        }

        while (*str == ' ' || *str == '\n' || *str == '\t' || *str == ',') {
            ++str;
        }

        char* data = new char[default_size];
        for (int i = 0; i < default_size; ++i) {
            data[i] = '\0';
        }
        int data_len = 0;

        if (is_digit (*str)) {
            data[data_len] = *str;
            ++data_len;
            ++str;
            while (is_digit (*str) || *str == '.') {
                data[data_len] = *str;
                ++data_len;
                ++str;
            }
            nodes[n_nodes].type = NUMBER;
        }
        else {
            switch (*str) {
                case 'd': {
                    if (strncmp (str, "dictate", 7) == 0) {
                        nodes[n_nodes].type = DEFINITION;
                        data = "dictate";
                        str += 7;
                    }
                    else if (strncmp (str, "dead", 4) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[DEAD].name;
                        str += states[DEAD].length;
                    }
                    else if (strncmp (str, "disloyal", 9) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[DISLOYAL].name;
                        str += states[DISLOYAL].length;
                    }
                    else if (strncmp (str, "destroyed", 9) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[DESTROYED].name;
                        str += states[DESTROYED].length;
                    }
                    else if (strncmp (str, "dishonest", 9) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[DISHONEST].name;
                        str += states[DISHONEST].length;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case ':': {
                    nodes[n_nodes].type = OPEN_BR;
                    data = ":";
                    str += 1;
                    break;
                }
                case '.': {
                    nodes[n_nodes].type = CLOS_BR;
                    data = ".";
                    str += 1;
                    break;
                }
                case '(': {
                    nodes[n_nodes].type = OPEN_EQ;
                    data = "(";
                    str += 1;
                    break;
                }
                case ')': {
                    nodes[n_nodes].type = CLOS_EQ;
                    data = ")";
                    str += 1;
                    break;
                }
                case '\"': {
                    nodes[n_nodes].type = COND_DIVIDER;
                    data = "\"";
                    str += 1;
                    break;
                }
                case 'p': {
                    if (strncmp (str, "person", 6) == 0) {
                        nodes[n_nodes].type = VARIABLE;
                        data = "person";
                        str += 6;
                    }
                    else if (strncmp (str, "punish", 6) == 0) {
                        nodes[n_nodes].type = DECR;
                        data = "punish";
                        str += 6;
                    }
                    else if (strncmp (str, "mandate", 7) == 0) {
                        nodes[n_nodes].type = PRINT;
                        data = "mandate";
                        str += 7;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'n': {
                    if (strncmp (str, "now", 3) == 0) {
                        nodes[n_nodes].type = ASSIGN;
                        data = "now";
                        str += 3;
                    }
                    else if (strncmp (str, "not", 3) == 0) {
                        nodes[n_nodes].type = NOT_EQUAL;
                        data = "not";
                        str += 3;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'i': {
                    if (strncmp (str, "if", 2) == 0) {
                        nodes[n_nodes].type = IF;
                        data = "if";
                        str += 2;
                    }
                    else if (strncmp (str, "is", 2) == 0) {
                        nodes[n_nodes].type = IS_EQUAL;
                        data = "is";
                        str += 2;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'w': {
                    if (strncmp (str, "while", 5) == 0) {
                        nodes[n_nodes].type = WHILE;
                        data = "while";
                        str += 5;
                    }
                    else if (strncmp (str, "worse", 5) == 0) {
                        nodes[n_nodes].type = IS_LESS;
                        data = "worse";
                        str += 5;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'o': {
                    if (strncmp (str, "otherwise", 9) == 0) {
                        nodes[n_nodes].type = CONDITION;
                        data = "otherwise";
                        str += 9;
                    }
                    else if (strncmp (str, "of", 2) == 0) {
                        nodes[n_nodes].type = ARGUMENTS;
                        data = "of";
                        str += 2;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'h': {
                    if (strncmp (str, "honor", 5) == 0) {
                        nodes[n_nodes].type = INCR;
                        data = "honor";
                        str += 5;
                    }
                    else if (strncmp (str, "happy", 5) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[HAPPY].name;
                        str += states[HAPPY].length;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case '+': {
                    nodes[n_nodes].type = PLUS;
                    data = "+";
                    str += 1;
                    break;
                }
                case '-': {
                    nodes[n_nodes].type = MINUS;
                    data = "-";
                    str += 1;
                    break;
                }
                case '*': {
                    nodes[n_nodes].type = MULT;
                    data = "*";
                    str += 1;
                    break;
                }
                case '/': {
                    nodes[n_nodes].type = DIVIDE;
                    data = "/";
                    str += 1;
                    break;
                }
                case 'b': {
                    if (strncmp (str, "better", 6) == 0) {
                        nodes[n_nodes].type = IS_BIGGER;
                        data = "better";
                        str += 6;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'c': {
                    if (strncmp (str, "cause", 5) == 0) {
                        nodes[n_nodes].type = CALL;
                        data = "cause";
                        str += 5;
                    }
                    else if (strncmp (str, "confused", 8) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[CONFUSED].name;
                        str += states[CONFUSED].length;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 's': {
                    if (strncmp (str, "summon", 6) == 0) {
                        nodes[n_nodes].type = RETURN;
                        data = "summon";
                        str += 6;
                    }
                    else if (strncmp (str, "sad", 3) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[SAD].name;
                        str += states[SAD].length;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'a': {
                    if (strncmp (str, "afraid", 6) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[AFRAID].name;
                        str += states[AFRAID].length;
                    }
                    else if (strncmp (str, "angry", 5) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[ANGRY].name;
                        str += states[ANGRY].length;
                    }
                    else if (strncmp (str, "and", 3) == 0) {
                        nodes[n_nodes].type = LAST_ARG;
                        data = "and";
                        str += 3;
                    }
                    else if (strncmp (str, "a ", 2) == 0) {
                        str += 2;
                        while (*str == ' ') {
                            ++str;
                        }
                        int scanfed = 0;

                        while (not_divider (*str)) {
                            ++str;
                            ++scanfed;
                        }

                        itoa_ (scanfed, data);
                        nodes[n_nodes].type = NUMBER;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'f': {
                    if (strncmp (str, "furious", 7) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[FURIOUS].name;
                        str += states[FURIOUS].length;
                    }
                    if (strncmp (str, "forgiven", 8) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[FORGIVEN].name;
                        str += states[FORGIVEN].length;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'l': {
                    if (strncmp (str, "loyal", 5) == 0) {
                        nodes[n_nodes].type = NUMBER;
                        data = states[LOYAl].name;
                        str += states[LOYAl].length;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                case 'k': {
                    if (strncmp (str, "know", 4) == 0) {
                        nodes[n_nodes].type = OUT;
                        data = "know";
                        str += 4;
                    }
                    else {
                        now_parse_that_long_long_name
                    }
                    break;
                }
                default: {
                    now_parse_that_long_long_name
                }
            }
        }

        nodes[n_nodes].name = data;
        ++n_nodes;
    }

    return nodes;
}

#endif