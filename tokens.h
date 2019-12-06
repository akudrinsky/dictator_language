#ifndef TOKENS                           //flag that tree was defined only once in all project
#define TOKENS

struct for_names {
    char* *names;
    int amount;

    explicit for_names (int size = 30);
    void append (char* element);
    ~for_names ();
};

for_names::for_names(int size) {
    names = new char*[size];
    amount = 0;
}

void for_names::append(char* element) {
    names[amount] = element;
    ++amount;
}

for_names::~for_names() {

}

for_names* ids = new for_names;

struct token {
    char* name;
    const int code;
    const char n_args;
    const char length;
};

enum token_codes {
    UNKNOWN,
    PROGRAMM,
    BLOCK,
    VARLIST,
    ID,
    VARIABLE,

    DEFINITION,
    CALL,
    RETURN,

    NUMBER,
    ASSIGN,
    IF,
    CONDITION,
    WHILE,
    IS_EQUAL,
    IS_BIGGER,
    IS_LESS,
    NOT_EQUAL,
    ADD,
    SUB,                      //......
    MULT,
    DIVIDE,

    INCR,
    DECR,

    OPEN_BR,
    CLOS_BR,
    COND_DIVIDER
};

enum person_states {
    DEAD,                       //states
    DESTROYED,
    DISLOYAL,
    AFRAID,
    FURIOUS,
    ANGRY,
    SAD,
    DISHONEST,
    CONFUSED,
    FORGIVEN,
    LOYAl,
    HAPPY
};

const token tokens[] = {
        {"unknown", UNKNOWN,    0, 7},
        {"P",       PROGRAMM,   1, 1},
        {"D",       BLOCK,      2, 1},
        {"varlist", VARLIST,    2, 7},
        {"id",      ID,         1, 2},//
        {"person",  VARIABLE,   1, 6},//

        {"dictate", DEFINITION, 1, 7},//
        {"cause",   CALL,       2, 5},//
        {"summon",  RETURN,     0, 6},//

        {"number",  NUMBER,     0, 6},

        {"now",     ASSIGN,     2, 3},//

        {"if",      IF,         2, 2},//
        {"otherwise",CONDITION, 2, 4},

        {"while",   WHILE,      2, 5},

        {"is",      IS_EQUAL,   2, 2},
        {"better",  IS_BIGGER,  2, 6},
        {"worse",   IS_LESS,    2, 5},
        {"not",     NOT_EQUAL,  2, 3},

        {"+",       ADD,        2, 1},
        {"-",       SUB,        2, 1},
        {"*",       MULT,       2, 1},
        {"/",       DIVIDE,     2, 1},

        {"honor",   INCR,       1, 5},
        {"punish",  DECR,       1, 6},

        {":",       OPEN_BR,    -1, 1},
        {".",       CLOS_BR,    -1, 1},
        {"\"",      COND_DIVIDER,-1, 1},
};

const token states[] {
        {"0", DEAD, 0, 4},
        {"1", DESTROYED, 0, 9},
        {"2", DISLOYAL, 0, 8},
        {"3", AFRAID, 0, 6},
        {"4", FURIOUS, 0, 7},
        {"5", ANGRY, 0, 5},
        {"6", SAD, 0, 3},
        {"7", DISHONEST, 0, 9},
        {"8", CONFUSED, 0, 8},
        {"9", FORGIVEN, 0, 8},
        {"10", LOYAl, 0, 5},
        {"11", HAPPY, 0, 5},
};

#endif