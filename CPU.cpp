#include "stack.cpp"
#include "enumerate.h"
#include "exit_codes.h"
#include <math.h>
#include <unistd.h>

#define ESC "\033"
enum colors {
    B_BLACK = 40,
    B_RED = 41,
    B_GREEN = 42,
    B_YELLOW = 43,
    B_BLUE = 44,
    B_MAGENTA = 45,
    B_CYAN = 46,
    B_WHITE = 47,
};

int regs[10] = {};
int labels[10] = {};
int func[10] = {};
int RAM[200] = {};

int execute_command (FILE* pFile, Stack_t* stk, Stack_t* func_stk, short* size_value, int* byte_now, FILE* output_f);
int preprocessor (char program_code[] = "./default/default_code.kdr");
void resetcolor (void);
void set_display_atrib (int color);

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Executes program written as a binary file by asm.cpp
//!
//! @param [in] program_code - name of binary file
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
int processor (char program_code[] = "./default/default_code.kdr", char output[] = "output.txt") {
    Stack_t stk = {};
    St_construct (&stk);

    Stack_t func_stk = {};
    St_construct (&func_stk);

    //output with "out"
    FILE* output_f = fopen (output, "w");
    ASSERT ( output_f != nullptr)

    //sleep (1);
    int mistake = preprocessor (program_code);
    if (mistake != 0) {
        return mistake;
    }
    //printf ("reg[%c] = %d\n", 1+'a', regs[0]);

    //read the input
    FILE* pFile = fopen (program_code, "rb");
    ASSERT ( pFile != nullptr)

    //read beginning info about version
    char info[5] = "";
    fread (info, sizeof(char), 5, pFile);

    if (strcmp (info, "kdr4") != 0) {
        printf ("Code version is outdated. Need to rewrite code.\n");
        assembler ();
    }

    //read number of code volume in file
    int n_bytes = 0;
    fread (&n_bytes, sizeof(int), 1, pFile);

    //main part
    int byte_now = 0;
    while (byte_now < n_bytes) {
        short size_arg = 0;
        //printf ("(pr) byte_now: %d\n", byte_now);
        //printf ("reg[%c] = %d\n", 1+'a', regs[1]);
        //printf ("reg[%c] = %d\n", 0+'a', regs[0]);
        mistake = execute_command (pFile, &stk, &func_stk, &size_arg, &byte_now, output_f);
        if (mistake != 0) {
            return mistake;
        }
        byte_now = byte_now + size_arg;
        ++byte_now;
        //printf ("(af) byte_now: %d\n", byte_now);
    }

    //make all constants 0 for second usage
    for (int i = 0; i < 10; ++i) {
        regs[i] = 0;
        labels[i] = 0;
        func[i] = 0;
    }

    Stack_Destruct (&stk);
    Stack_Destruct (&func_stk);

    fclose (output_f);
    fclose (pFile);

    return 0;
}

bool search_name (label lb, char* s_name) {
    //for (int i = 0; i < )
    return false;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Executes command from processor
//!
//! @param [in] pFile - pointer to file where all the commands are
//! @param [in] stk - pointer to stack with commands (order)
//! @param [in] with_value - if the command required value after itself
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
int execute_command (FILE* pFile, Stack_t* stk, Stack_t* func_stk, short* size_arg, int* byte_now, FILE* output_f) {

    int cmd_now = 0;
    fread(&cmd_now, sizeof(char), 1, pFile);
    //printf ("cmd_now: %2d, i.e. ", cmd_now);

    #define DEF_CMD(name, number, bytes, cpu_code, asm_code)\
        case kdr_##name:                                    \
            printf ("%s\n", #name);\
            cpu_code;                                       \
            * size_arg = bytes - 1;                       \
            break;

    switch (cmd_now) {
        #include "commands.h"

        default: {
            printf("Unknown command while executing code: %d\n", cmd_now);
            return (unknown_command);
        }
    }

    #undef DEF_CMD

    return 0;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Makes labels from binary file by asm.cpp
//!
//! @param [in] program_code - name of binary file
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
int preprocessor (char program_code[]) {

    #define DEF_CMD(name, number, bytes, cpu_code, asm_code)\
        if (cmd_now == kdr_##name) {                        \
            size_arg = bytes - 1;                           \
            fseek (pFile, bytes - 1, SEEK_CUR);             \
        }

    //read the input
    FILE* pFile = fopen (program_code, "rb");
    ASSERT ( pFile != nullptr)

    //read beginning info about version
    char info[5] = "";
    fread (info, sizeof(char), 5, pFile);

    if (strcmp (info, "kdr4") != 0) {
        printf ("Code version is outdated. Need to rewrite code.\n");
        assembler ();
    }

    //read number of code volume in file
    int n_bytes = 0;
    fread (&n_bytes, sizeof(int), 1, pFile);

    int cmd_now = 0;

    //main part
    for (int byte_now = 0; byte_now < n_bytes; ++byte_now) {
        int size_arg = 0;
        fread(&cmd_now, sizeof(char), 1, pFile);

        #include "commands.h"

        if (cmd_now == kdr_label) {
            char value = 0;
            fseek (pFile, -1, SEEK_CUR);
            fread (&value, sizeof (char), 1, pFile);
            if (labels[value] != 0) {
                printf ("You shouldn't use label (%c) twice!", (value + 'a'));
                return (label_twice);
            }
            else
                labels[value] = byte_now + 2;
        }

        if (cmd_now == kdr_func) {
            char value = 0;
            fseek (pFile, -1, SEEK_CUR);
            fread (&value, sizeof (char), 1, pFile);
            if (func[value] != 0) {
                printf ("You shouldn't write function (%c) twice!", (value + 'a'));
                return (func_twice);
            }
            else
                func[value] = byte_now + 2;
        }

        byte_now += size_arg;
    }

    fclose (pFile);

    return 0;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Resets colour of video RAM cursor
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void resetcolor (void) {
    printf(ESC "[0m");
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! changes colour of cursor in video RAM
//!
//! @param [in] color - color to be in video RAM
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void set_display_atrib (int color) {
    printf(ESC "[%dm", color);
}

/*
//#define resetcolor() printf(ESC "[0m")
//#define set_display_atrib(color) printf(ESC "[%dm", color)
const int B_BLACK = 40;
const int B_RED = 41;
const int B_GREEN = 42;
const int B_YELLOW = 43;
const int B_BLUE = 44;
const int B_MAGENTA = 45;
const int B_CYAN = 46;
const int B_WHITE = 47;
 */