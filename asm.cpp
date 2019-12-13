#include "my_lyb.h"
#include "enumerate.h"
#include <cstdlib>
#include "exit_codes.h"

const int num_labels = 15;

struct label {
    char* names[num_labels];
    int num_now;
    bool was_def[num_labels];
};

int parse_RAM (char* *cmd_ptr, FILE* pFile);

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Makes binary file out of real, makes backup version of previous file if needed
//!
//! @param [in] program_name - name text program
//! @param [in] program_code - name of binary file
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
int assembler (char input[] = "input.txt",
                char program_name[] = "./default/default_program.txt",
                char program_code[] = "./default/default_code.kdr") {

    label labels = {};
    label funcs = {};

    //input with "in"
    FILE* input_f = fopen (input, "r");
    ASSERT (input_f != nullptr)

    #define DEF_CMD(name, с_number, bytes, cpu_code, asm_code)    \
    if (strcmp (word, #name) == 0) {                    \
        unknown_command = false;                        \
        *cmd_ptr = с_number;                            \
        ++cmd_ptr;                                      \
        asm_code;                                       \
    }

    //file with program in text
    FILE* pFile = fopen (program_name, "r");
    ASSERT (program_name != nullptr)

    //file with program in code
    FILE* code = fopen (program_code, "wb");
    ASSERT ( code != nullptr)
    char info[5] = "kdr4";
    fwrite (info, sizeof (char), 5, code);
    fseek (code, sizeof (int), SEEK_CUR);

    //array with machine code (to the final writing to program_code.kdr)
    int max_symbols = count_symbols (pFile);
    char* commands = (char*)calloc (max_symbols, sizeof (char));

    char* cmd_ptr = commands;                       //now which byte
    while (true) {
        char word[15] = {'\0'};
        int value = 0;

        short number = 0;

        number = fscanf (pFile, "%s", word);         //number of successfully scanfed variales

        // if we scanned nothing, then it's the end of file
        if (number == EOF) {
            break;
        }

        bool unknown_command = true;

        #include "commands.h"

        //if it was not checked, then command is unknown
        if (unknown_command) {
            printf("There was an unknown command %s with %d arguments after it\n\tBad file (kdr) was removed, backup version exists\n", word, number - 1);
            remove (program_code);
            return (unknown_command);
        }
    }

    fwrite (commands, sizeof (char), (cmd_ptr - commands), code);
    int commands_len = cmd_ptr - commands;

    fseek (code, sizeof(char[5]), SEEK_SET);
    fwrite (&commands_len, sizeof(int), 1, code);

    fclose (pFile);
    fclose (code);

    fclose (input_f);


    for (int i = 0; i < commands_len; ++i) {
        printf ("[%3d]=%3d  ", i, commands[i]);
        if (i % 10 == 9)
            printf ("\n");
    }
    printf ("\n");


    //free everything
    for (int i = 0; i < num_labels; ++i) {
        free (labels.names[i]);
    }
    for (int i = 0; i < num_labels; ++i) {
        free (funcs.names[i]);
    }
    free (commands);

    return 0;
#undef DEF_CMD
}

int parse_RAM (char* *cmd_ptr, FILE* pFile) {
    const int max_size_cmd = 15;
    char* l_name = (char*) calloc (max_size_cmd, sizeof (char));
    fscanf (pFile, "%s", l_name);
    //printf ("%s\n", l_name);

    int n = strnlen (l_name, max_size_cmd);

    if (l_name[0] != '[' || l_name[n-1] != ']') {
        printf ("Wrong format of popm (brackets): %s\n", l_name);
        return (no_args);
    }

    if ((l_name[1] >= 'a' && l_name[1] <= 'k') && l_name[2] == 'x') {           //then there is register
        *(*cmd_ptr) = l_name[1] - 'a';
        ++(*cmd_ptr);
        if (l_name[3] == '+') {                                                 //there is reg and num
            char* arg = (char*) calloc (n - 4, sizeof (char));
            for (int i = 0; i < n - 5; ++i) {
                arg[i] = l_name[i+4];
            }
            bool is_number = false;
            int value = atoi_ (arg, &is_number);
            if (is_number) {                                                    //num is all right
                if (value > 200) {
                    printf ("Our device doesn't have enough RAM\n");
                    return (not_ram);
                }
                else {                                                          //maybe we have RAM address like that
                    *(*cmd_ptr) = (char) value;
                    ++(*cmd_ptr);
                }
            }
            else {                                                              //num is npt all right
                printf ("Wrong format of popm (nan): %s\n", l_name);
                return (no_args);
            }

            //printf ("%s\n", arg);
            free (arg);
        }
        else if (l_name[3] != ']') {                                            //unkwown format
            printf ("Wrong format of popm (after reg): %s\n", l_name);
            return (no_args);
        }
        else {
            *(*cmd_ptr) = -1;
            ++(*cmd_ptr);
        }
    }
    else {                                                                      //no register
        *(*cmd_ptr) = -1;
        ++(*cmd_ptr);

        char* arg = (char*) calloc (n, sizeof (char));
        for (int i = 0; i < n - 2; ++i) {
            arg[i] = l_name[i+1];
        }
        //printf ("%s\n", arg);

        bool is_number = false;
        int value = atoi_ (arg, &is_number);
        if (is_number) {                                                    //num is all right
            if (value > 200) {
                printf ("Our device doesn't have enough RAM\n");
                return (not_ram);
            }
            else {                                                          //maybe we have RAM address like that
                *(*cmd_ptr) = (char) value;
                ++(*cmd_ptr);
            }
        }
        else {                                                              //num is npt all right
            printf ("Wrong format of popm (nan): %s\n", l_name);
            return (no_args);
        }

        free (arg);
    }

    free (l_name);

    return 0;
}