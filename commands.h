//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//!    Makes operating from code to name of command and vice versa more easy and global
//!    Name, code number, size in code (in bytes, with arguments), code to insert in CPU, code to insert in ASM
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

#define PUSH(value) St_push(stk, (value));
#define POP St_pop(stk);

#define DEF_JUMP_ASM(where)\
    char* l_name = (char*) calloc (10, sizeof (char));                      \
    fscanf (pFile, "%s", l_name);                                           \
                                                                            \
    bool was_earlier = false;                                               \
    for (int i = 0; i < where.num_now; ++i) {                               \
        if (strcmp (l_name, where.names[i]) == 0) {                         \
            was_earlier = true;                                             \
            *cmd_ptr = i;                                                   \
            ++cmd_ptr;                                                      \
        }                                                                   \
    }                                                                       \
                                                                            \
    if (!was_earlier) {                                                     \
        where.names[where.num_now] = l_name;                                \
        where.num_now++;                                                    \
                                                                            \
        *cmd_ptr = where.num_now - 1;                                       \
        ++cmd_ptr;                                                          \
    }                                                                       \

#define DEF_LABEL_ASM(where)\
    char* l_name = (char*) calloc (10, sizeof (char));                     \
    fscanf (pFile, "%s", l_name);                                          \
    bool was_earlier = false;                                              \
    for (int i = 0; i < where.num_now; ++i) {                              \
        if (strcmp (l_name, where.names[i]) == 0) {                        \
            if (where.was_def[i]) {                                        \
                printf ("You have used one ",#where , "description twice! That's forbidden\n");\
                return (func_twice);                                         \
            }                                                              \
            else {                                                         \
                was_earlier = true;                                        \
                where.was_def[i] = true;                                   \
                *cmd_ptr = i;                                              \
                ++cmd_ptr;                                                 \
            }                                                              \
        }                                                                  \
    }                                                                      \
                                                                           \
    if (!was_earlier) {                                                    \
        where.names[where.num_now] = l_name;                               \
        where.num_now++;                                                   \
                                                                           \
        *cmd_ptr = where.num_now - 1;                                      \
        ++cmd_ptr;                                                         \
    }                                                                      \


//00000 000
DEF_CMD (dump, 0, 1, {
    Stack_OK (*stk);
}, {})

//00001 001 (to ram, reg, int)
DEF_CMD (push, 1, 5, {
    int value = 0;
    fread (&value, sizeof(int), 1, pFile);
    //printf ("pushing %d\n", value / 10000);
    PUSH (value)
}, {
    if (fscanf (pFile, "%d", &value) == 0) {
        printf ("You are trying to push something wrong\n");
        return (push_problem);
    }
    value = value * 10000;
    char* value_c = int_to_code (value);
    int i = 0;
    while (i < 4) {
        *cmd_ptr = value_c[i];
        ++cmd_ptr;
        ++i;
    }
    free (value_c);
})

//00010 001 (from ram, reg, int)
DEF_CMD (pop, 2, 1, {
    POP
}, {})

//00011 000
DEF_CMD (add, 3, 1, {
    if (stk->size < 2) {
        printf("You are trying to add non-existing elements\n");
    return(non_exist_elem);
    } else {
        int op_2 = POP
        int op_1 = POP
        PUSH (op_2 + op_1)
    }
}, {})

//00100 000
DEF_CMD (sub, 4, 1, {
    if (stk->size < 2) {
        printf("You are trying to subtract non-existing elements\n");
        return (non_exist_elem);
    } else {
        int op_2 = POP
        int op_1 = POP
        PUSH (op_1 - op_2);
    }
}, {})

//00101 000
DEF_CMD (mul, 5, 1, {
    if (stk->max_size < 2) {
        printf("You are trying to multiply non-existing elements\n");
        return (non_exist_elem);
    } else {
        int op_2 = POP
        int op_1 = POP
        PUSH ((op_1 / 100) * (op_2 / 100));
    }
}, {})

//00110 000
DEF_CMD (div, 6, 1, {
    if (stk->size < 2) {
        printf("You are trying to divide non-existing elements\n");
        return (non_exist_elem);
    } else {
        long int op_2 = POP
        if (op_2 == 0) {
            printf("You are trying to divide by zero\n");
            return (div_zero);
        }
        long int op_1 = POP
        long long int result = (op_1 * 10000) / op_2;
        PUSH ((int)result);
    }
}, {})

//00111 000
DEF_CMD (out, 7, 1, {
    if (stk->size < 1) {
        printf ("Don't out non-existing elements!\n");
        return (non_exist_elem);
    }
    int op_1 = POP
    bool is_positive = true;
    if (op_1 < 0)
        is_positive = false;
    if (is_positive)
        fprintf (output_f, "Result: %d.%d\n", op_1 / 10000, op_1 % 10000);
    else
        fprintf (output_f, "Result: -%d.%d\n", -op_1 / 10000, -op_1 % 10000);
}, {})

//01000 000
DEF_CMD (in, 8, 5, {
    int value = 0;
    fread (&value, sizeof(int), 1, pFile);
    PUSH (value);
}, {
    if (fscanf (input_f, "%d", &value) == 0) {
        printf ("You are trying to push (in) something wrong\n");
        return (push_problem);
    }
    value = value * 10000;
    char* value_c = int_to_code (value);
    int i = 0;
    while (i < 4) {
        *cmd_ptr = value_c[i];
        ++i;
        ++cmd_ptr;
    }
})

//00010 010
DEF_CMD (popr, 9, 2, {                       //stack.pop to register
    char value = 0;
    fread (&value, sizeof (char), 1, pFile);
    //printf ("regs[%c] = %d\n", value + 'a', regs[value]);
    regs[value] = POP;
    printf ("reg[%c] = %d\n", value+'a', regs[value]);
}, {
    char reg_name[3] = "";
    fscanf (pFile, "%s", reg_name);

    if ((reg_name[0] - 'a' < 0) || (reg_name[0] - 'a' > 9)) {
        printf ("You are trying to operate with non-existing register: %s", reg_name);
        return (non_exist_elem);
    }
    if (reg_name[1] != 'x') {
        printf ("You are trying to operate with non-existing register: %s", reg_name);
        return (non_exist_elem);
    }
    if (reg_name[2] != '\0') {
        printf ("You are trying to operate with non-existing register: %s", reg_name);
        return (non_exist_elem);
    }

    *cmd_ptr = reg_name[0] - 'a';
    ++cmd_ptr;
})

//00001 010
DEF_CMD (pushr, 10, 2, {                         //register to stack
    int value = 0;
    fread (&value, sizeof (char), 1, pFile);
    PUSH (regs[value]);
}, {
    char reg_name[2] = "";
    fscanf (pFile, "%s", reg_name);

    if ((reg_name[0] - 'a' < 0) || (reg_name[0] - 'a' > 9)) {
        printf ("You are trying to operate with non-existing register: %s", reg_name);
        return (non_exist_elem);
    }
    if (reg_name[1] != 'x') {
        printf ("You are trying to operate with non-existing register: %s", reg_name);
        return (non_exist_elem);
    }
    if (reg_name[2] != '\0') {
        printf ("You are trying to operate with non-existing register: %s", reg_name);
        return (non_exist_elem);
    }

    *cmd_ptr = reg_name[0] - 'a';
    ++cmd_ptr;
})

//01001 000
DEF_CMD (label, 11, 2, {
    //(*byte_now) += 1;
    fseek (pFile, 1, SEEK_CUR);
}, {
    DEF_LABEL_ASM (labels)
})

//01001 001
DEF_CMD (jump, 12, 2, {
    char value = 0;
    fread (&value, sizeof (char), 1, pFile);
    (*byte_now) = labels[value] - 2;                                //size_arg and command
    //printf ("(th) byte_now: %d\n", *byte_now);
    fseek (pFile, (labels[value] + 5 * sizeof (char) + sizeof (int)), SEEK_SET);
}, {
    DEF_JUMP_ASM(labels)
})

#define DEF_CMP_CMD(name_op, number_op, operation_op)\
DEF_CMD (name_op, number_op, 2, {                                           \
    if (stk->size < 2) {                                                    \
        printf("You are trying to compare non-existing elements in ");      \
        printf(#name_op);                                                   \
        printf("\n");                                                       \
        return (non_exist_elem);                                              \
    } else {                                                                \
        int op_2 = POP                                                      \
        int op_1 = POP                                                      \
                                                                            \
        if (op_2 - op_1 operation_op 0) {                                   \
            char value = 0;                                                 \
            fread (&value,                                                  \
            sizeof (char), 1, pFile);                                       \
                                                                            \
            * byte_now = labels[value] - 2;                                 \
            fseek (pFile, labels[value]                                     \
            + 5 * sizeof (char) + sizeof (int), SEEK_SET);                  \
        }                                                                   \
        else {                                                              \
            fseek (pFile, 1, SEEK_CUR);                                     \
        }                                                                   \
    }                                                                       \
}, {                                                                        \
    DEF_JUMP_ASM (labels)                                                   \
})

DEF_CMP_CMD (ja, 13, >)                 //if last element in stack is bigger than pre-last, the rest are analogous
                                                    //01001 010
DEF_CMP_CMD (jae, 14, >=)                   //01001 011
DEF_CMP_CMD (jb, 15, <)                     //01001 100
DEF_CMP_CMD (jbe, 16, <=)                   //01001 101
DEF_CMP_CMD (je, 17, ==)                    //01001 110
DEF_CMP_CMD (jne, 18, !=)                   //01001 111

//01001 000
DEF_CMD (print, 19, 11, {
    char value[11] = "";
    fread (value, sizeof (char), 10, pFile);
    fprintf (output_f, value);
    fprintf (output_f, "\n");
}, {
    char l_name[10] = "";
    fscanf (pFile, "%s", l_name);

    for (int i = 0; i < 10; ++i) {
        *cmd_ptr = l_name[i];
        ++cmd_ptr;
    }
})

//01010 000
DEF_CMD (end, 20, 1, {
    fprintf(output_f, "Program was manually finished\n");
    * byte_now = 10000000;
}, {})

//01011 000
DEF_CMD (sqrt, 21, 1, {
    if (stk->size < 1) {
        printf("You are trying to sqrt non-existing element\n");
        return (non_exist_elem);
    } else {
        int op_1 = POP
        PUSH (sqrt (op_1) * 100);
    }
}, {})

//01100 000
DEF_CMD (func, 22, 2, {                                     //f1, f2...
    (*byte_now) += 1;
    fseek (pFile, 1, SEEK_CUR);
}, {
    DEF_LABEL_ASM (funcs)
})

//01100 001
DEF_CMD (ret, 23, 1, {                      //jump to number from func_stk
    int value = 0;
    value = St_pop (func_stk);
    *byte_now = value;
    printf ("returned to %d\n", value);
    fseek (pFile, (value + 5 * sizeof (char) + sizeof (int) + 1), SEEK_SET);
}, {
})

//01100 010
DEF_CMD (call, 24, 2, {
    char value = 0;
    fread (&value, sizeof (char), 1, pFile);
    St_push (func_stk, *byte_now + 1);                                      //it was +2
    //printf ("for ret byte_now: %d\n", *byte_now+2);
    (*byte_now) = func[value] - 2;                                //size_arg and command
    //printf ("(th) byte_now: %d\n", *byte_now);
    fseek (pFile, (func[value] + 5 * sizeof (char) + sizeof (int)), SEEK_SET);
}, {
    DEF_JUMP_ASM (funcs)
})

//00010 010
DEF_CMD (popm, 25, 3, {                       //stack.pop to register
    char* args = (char*) calloc (4, sizeof (char));
    fread (args, sizeof (char), 2, pFile);

    if (args[0] == (char)-1) {
        if (args[1] == (char)-1) {
            printf ("You gave no arguments\n");
            return (no_args);
        }
        else {
            RAM[args[1]] = POP;
            printf ("RAM[%d] now: %d\n", args[1], RAM[args[1]]);
        }
    }
    else {
        if (args[1] == (char)-1) {
            RAM[regs[args[0]] / 10000] = POP;
            printf ("RAM[%d] now: %d\n", regs[args[0]] / 10000, RAM[args[1]]);
        }
        else {
            RAM[regs[args[0]] / 10000 + args[1]] = POP;
            printf ("RAM[%d] now: %d\n", regs[args[0]] / 10000 + args[1], RAM[regs[args[0]] / 10000 + args[1]]);
        }
    }

    free (args);
    //sleep (1);
}, {
    int mistake = parse_RAM (&cmd_ptr, pFile);

    if (mistake != 0) {
        return mistake;
    }
})

//00001 010
DEF_CMD (pushm, 26, 3, {                         //RAM to stack
    char* args = (char*) calloc (2, sizeof (char));
    fread (args, sizeof (char), 2, pFile);
    if (args[0] == (char)-1) {
        if (args[1] == (char)-1) {
            printf ("You gave no arguments\n");
            return (non_exist_elem);
        }
        else {
            PUSH (RAM[args[1]]);
        }
    }
    else {
        if (args[1] == (char)-1) {
            PUSH (RAM[regs[args[0]] / 10000]);
        }
        else {
            PUSH (RAM[regs[args[0]] / 10000 + args[1]]);
        }
    }

    free (args);
    //sleep (1);
}, {
    int mistake = parse_RAM (&cmd_ptr, pFile);

    if (mistake != 0) {
        return mistake;
    }
})

//01100 001
DEF_CMD (displ, 27, 1, {                      //display first 10*10 values of RAM as a video memory
    //printf ("displ\n");
    for (int i = 0; i < 100; ++i) {
        switch (RAM[i]) {
            case 10000: {
                set_display_atrib(B_BLACK);
                printf(" ");
                resetcolor();
                break;
            }
            case 20000: {
                set_display_atrib(B_RED);
                printf(" ");
                resetcolor();
                break;
            }
            case 30000: {
                set_display_atrib(B_GREEN);
                printf(" ");
                resetcolor();
                break;
            }
            case 40000: {
                set_display_atrib(B_YELLOW);
                printf(" ");
                resetcolor();
                break;
            }
            case 50000: {
                set_display_atrib(B_BLUE);
                printf(" ");
                resetcolor();
                break;
            }
            case 60000: {
                set_display_atrib(B_MAGENTA);
                printf(" ");
                resetcolor();
                break;
            }
            case 70000: {
                set_display_atrib(B_CYAN);
                printf(" ");
                resetcolor();
                break;
            }
            case 80000: {
                set_display_atrib(B_WHITE);
                printf(" ");
                resetcolor();
                break;
            }
            default: {
                printf(" ");
            }
        }

        if (i % 10 == 9) {
            printf ("\n");
        }
    }
    usleep (400000);
}, {
})

//01100 001
DEF_CMD (clrscr, 28, 1, {
    printf ("\n\n\n\n\n\n\n\n\n\n");
    //usleep (500000);
    for (int i = 0; i < 100; ++i) {
        RAM[i] = 0;
    }
}, {
})

//01100 001
DEF_CMD (print_RAM, 29, 1, {
    for (int i = 0; i < 100; ++i) {
        printf ("[%3d]=%6d  ", i, RAM[i]);
        if (i % 10 == 9)
        printf ("\n");
        }
    printf ("\n");
}, {
})
