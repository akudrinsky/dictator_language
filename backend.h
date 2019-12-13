#ifndef BACKEND
#define BACKEND

#include "tree.h"

// fx - for memory usage

struct b_end {
    FILE* dist;
    node* main_nd;
    char cond_number;
    for_names* local_vars;

    explicit b_end (FILE* req_dist, node* root);

    void write (const char* info1, const char* info2 = "", const char* info3 = "");
    int new_cond ();
    bool reg_name (node* nd);

    bool all ();
    bool block (node* nd);
    bool func_def (node* nd);
    bool func_return (node* nd);
    bool func_call (node* nd);
    bool variable (node* nd);

    bool instructions (node* nd);

    bool assign (node* nd);

    bool condition (node* nd);
    bool cycle (node* nd);

    bool push (node* nd);

    bool increm (node* nd);
    bool decrem (node* nd);

    bool out (node* nd);

    bool equation (node* nd);
};

b_end::b_end (FILE *req_dist, node *root) {
    dist = req_dist;
    main_nd = root;
    cond_number = 'a';
    local_vars = nullptr;
}

void b_end::write (const char *info1, const char *info2, const char *info3) {
    fprintf (dist, "%s%s%s", info1, info2, info3);
}

int b_end::new_cond () {
    cond_number++;
    return cond_number - 1;
}

bool b_end::reg_name (node *nd) {
    if (nd->data == nullptr) {
        err_info ("problems in reg_name\n");
        return false;
    }
    int index = local_vars->search_name (nd->data);
    if (index == -1) {
        write ("WTF!??!?!?");
    }
    else {
        fprintf (dist, "%d", local_vars->search_name (nd->data));
    }
    return true;
}

bool b_end::all () {
    if (main_nd == nullptr) {
        err_info ("problems in backend: all (1)\n");
        return false;
    }


    if (main_nd->type == BLOCK) {
        flag
        return block (main_nd);
    }
    else {
        err_info ("problems in backend: all (2)\n");
        return false;
    }
}

bool b_end::block (node *nd) {
    bool have_problems = false;

    if (nd == nullptr) {
        err_info ("problems in backend: block (1)\n");
        return false;
    }

    if (nd->type != BLOCK) {
        err_info ("problems in backend: block (2)\n");
        return false;
    }

    if (nd->left != nullptr) {
        have_problems = have_problems || block (nd->left);
    }

    if (nd->right != nullptr) {
        if (nd->right->type == DEFINITION) {
            have_problems = have_problems || func_def (nd->right);
        }
        else if (nd->right->type == VARIABLE) {
            have_problems = have_problems || variable (nd->right);
        }
    }
    else {
        err_info ("problems in backend: block (3)\n");
        return false;
    }

    return !have_problems;
}

bool b_end::func_def (node *nd) {
    bool have_problems = false;

    if (nd == nullptr) {
        err_info ("problems in backend: func_def (1)\n");
        return false;
    }

    if (nd->type != DEFINITION) {                                       //current node - func definition
        err_info ("problems in backend: func_def (2)\n");
        return false;
    }

    for_names* local_names = new for_names;
    if (nd->is_left ()) {
        node* tmp = nd->left;
        while (tmp->left != nullptr) {
            local_names->append (tmp->right->data);
            tmp = tmp->left;
        }
        local_names->append (tmp->right->data);
        local_names->initial_ammount++;
    }
    local_vars = local_names;
    local_vars->print();

    if (nd->right == nullptr) {
        err_info ("problems in backend: func_def (3)\n");
        return false;
    }
    else {
        node* id_nd = nd->right;

        write ("\nfunc ");
        write (id_nd->data);
        write ("\npushr ax\npush 5\nadd\npopr ax\n\n");

        int i = 0;
        node* tmp = nd;
        for (int i = 0; i < local_vars->amount; ++i) {
            fprintf (dist, "popm [ax+%d]\n", local_names->initial_ammount - i);
        }
        //write ("\npushr fx\npush 10\nadd\npopr fx\n");

        if (id_nd->right == nullptr) {
            err_info ("problems in backend: func_def (4)\n");
        }
        else {
            have_problems = have_problems || instructions (id_nd->right);
        }
    }

    return !have_problems;
}

bool b_end::func_return (node *nd) {
    if (nd == nullptr) {
        err_info ("problems in backend: func_return (1)\n");
        return false;
    }

    if (nd->type != RETURN) {                                       //current node - return
        err_info ("problems in backend: func_return (2)\n");
        return false;
    }

    for (int i = 0; i <= local_vars->initial_ammount; ++i) {
        fprintf (dist, "pushm [ax+%d]\n", i);
    }

    write ("\npushr ax\npush 5\nsub\npopr ax\n\nret\n");

    return true;
}

bool b_end::func_call (node *nd) {
    if (nd == nullptr) {
        err_info ("problems in backend: func_call (1)\n");
        return false;
    }

    if (nd->type != CALL) {
        err_info ("problems in backend: func_call (2)\n");
        return false;
    }

    if (nd->is_right()) {
        int num = 0;
        node* tmp = nd;
        while (tmp->left != nullptr) {
            tmp = tmp->left;
            push (tmp->right);
            ++num;
        }
        write ("call ", nd->right->data, "\n");

        for (int i = 0; i < num; ++i) {
            fprintf (dist, "popm [ax+%d]\n", i);
        }
    }
    else {
        err_info ("problems in backend: func_call (3)\n");
        return false;
    }

    return true;
}

bool b_end::variable (node *nd) {
    bool have_problems = false;

    if (nd == nullptr) {
        err_info ("problems in backend: variable (1)\n");
        return false;
    }

    if (nd->type != VARIABLE) {                                       //current node - variable
        err_info ("problems in backend: variable (2)\n");
        return false;
    }

    if (nd->right != nullptr) {
        local_vars->append (nd->right->data);
    }
    else {
        err_info ("problems in backend: variable (3)\n");
        return false;
    }

    return !have_problems;
}

bool b_end::instructions (node *nd) {
    bool have_problems = false;

    if (nd == nullptr) {
        err_info ("problems in backend: instructions (1)\n");
        return false;
    }

    if (nd->type != INSTRUCTION) {
        err_info ("problems in backend: instructions (2)\n");
        return false;
    }

    if (nd->left != nullptr) {
        have_problems = have_problems || instructions (nd->left);
    }

    if (nd->right != nullptr) {
        switch (nd->right->type) {
            case VARIABLE: {
                have_problems = have_problems || variable (nd->right);
                break;
            }
            case ASSIGN: {
                have_problems = have_problems || assign (nd->right);
                break;
            }
            case RETURN: {
                have_problems = have_problems || func_return (nd->right);
                break;
            }
            case CALL: {
                have_problems = have_problems || func_call (nd->right);
                break;
            }
            case IF: {
                have_problems = have_problems || condition (nd->right);
                break;
            }
            case INCR: {
                have_problems = have_problems || increm (nd->right);
                break;
            }
            case DECR: {
                have_problems = have_problems || decrem (nd->right);
                break;
            }
            case WHILE: {
                have_problems = have_problems || cycle (nd->right);
                break;
            }
            case OUT: {
                have_problems = have_problems || out (nd->right);
                break;
            }
            default: {
                err_info ("problems in backend: instructions (4)\n");
                err_info (nd->data);
            }
        }
    }
    else {
        err_info ("problems in backend: instructions (3)\n");
        return false;
    }

    return !have_problems;
}

bool b_end::assign (node *nd) {
    bool have_problems = false;

    if (nd == nullptr) {
        err_info ("problems in backend: variable (1)\n");
        return false;
    }

    if (nd->type != ASSIGN) {                                       //current node - variable
        err_info ("problems in backend: variable (2)\n");
        return false;
    }

    //write ("\npush ", nd->right->data, "\n");
    have_problems = have_problems || equation (nd->right);

    write ("popm [ax+");
    reg_name (nd->left);
    write ("]\n");

    return !have_problems;
}

bool b_end::condition (node *nd) {
    bool have_problems = false;
    char now_cond = cond_number + 1;

    if (nd == nullptr) {
        err_info ("problems in backend: condition (1)\n");
        return false;
    }

    if (nd->type != IF) {                                       //current node - variable
        err_info ("problems in backend: condition (2)\n");
        return false;
    }

    if (nd->left != nullptr) {
        node* cond = nd->left;

        if (cond->is_left() && cond->is_right()) {
            write ("\n");
            push (cond->left);
            push (cond->right);

            switch (cond->type) {
                case IS_EQUAL: {
                    fprintf (dist, "je %c\n", cond_number);
                    break;
                }
                case IS_BIGGER: {
                    fprintf (dist, "ja %c\n", cond_number);
                    break;
                }
                case IS_LESS: {
                    fprintf (dist, "jb %c\n", cond_number);
                    break;
                }
                case NOT_EQUAL: {
                    fprintf (dist, "jne %c\n", cond_number);
                    break;
                }
                default: {
                    err_info ("problems in backend: condition (5)\n");
                    return false;
                }
            }
        }
        else {
            err_info ("problems in backend: condition (4)\n");
            return false;
        }

        fprintf (dist, "jump %c\n\nlabel %c\n", cond_number + 1, cond_number);
        cond_number += 2;
    }
    else {
        err_info ("problems in backend: condition (3)\n");
        return false;
    }

    if (nd->is_right ()) {
        have_problems = have_problems || instructions (nd->right);
    }
    else {
        err_info ("problems in backend: condition (6)\n");
        return false;
    }

    fprintf (dist, "label %c\n", now_cond);

    return !have_problems;
}

bool b_end::cycle (node *nd) {
    bool have_problems = false;

    if (nd == nullptr) {
        err_info ("problems in backend: cycle (1)\n");
        return false;
    }

    if (nd->type != WHILE) {                                       //current node - variable
        err_info ("problems in backend: cycle (2)\n");
        return false;
    }

    if (nd->left != nullptr) {
        fprintf (dist, "\nlabel %c\n", cond_number);
        cond_number += 3;

        node* cond = nd->left;

        if (cond->is_left() && cond->is_right()) {
            write ("\n");
            push (cond->left);
            push (cond->right);

            switch (cond->type) {
                case IS_EQUAL: {
                    fprintf (dist, "je %c\n", cond_number - 2);
                    break;
                }
                case IS_BIGGER: {
                    fprintf (dist, "ja %c\n", cond_number - 2);
                    break;
                }
                case IS_LESS: {
                    fprintf (dist, "jb %c\n", cond_number - 2);
                    break;
                }
                case NOT_EQUAL: {
                    fprintf (dist, "jne %c\n", cond_number - 2);
                    break;
                }
                default: {
                    err_info ("problems in backend: cycle (5)\n");
                    return false;
                }
            }
            fprintf (dist, "jump %c\n", cond_number - 1);
        }
        else {
            err_info ("problems in backend: cycle (4)\n");
            return false;
        }

        fprintf (dist, "\nlabel %c\n", cond_number - 2);
    }
    else {
        err_info ("problems in backend: cycle (3)\n");
        return false;
    }

    if (nd->is_right ()) {
        have_problems = have_problems || instructions (nd->right);
        fprintf (dist, "jump %c\nlabel %c\n", cond_number - 3, cond_number - 1);
    }
    else {
        err_info ("problems in backend: cycle (6)\n");
        return false;
    }

    return !have_problems;
}

bool b_end::push (node *nd) {
    if (nd == nullptr) {
        err_info ("problems in backend: push (1)\n");
        return false;
    }
    if (nd->type == ID) {
        write ("pushm [ax+");
        reg_name (nd);
        write ("]\n");
    }
    else if (nd->type == NUMBER) {
        write ("push ", nd->data, "\n");
    }
    else {
        err_info ("problems in backend: push (2)\n");
        return false;
    }
}

bool b_end::increm (node *nd) {
    if (nd == nullptr) {
        err_info ("problems in backend: increm (1)\n");
        return false;
    }

    if (nd->type == INCR) {
        if (nd->right == nullptr) {
            err_info ("problems in backend: increm (3)\n");
            return false;
        }

        push (nd->right);

        write ("\npush 1\nadd\npopm [ax+");
        reg_name (nd->right);
        write ("]\n");

        return true;
    }
    else {
        err_info ("problems in backend: increm (2)\n");
        return false;
    }
}

bool b_end::decrem (node *nd) {
    if (nd == nullptr) {
        err_info ("problems in backend: decrem (1)\n");
        return false;
    }

    if (nd->type == DECR) {
        if (nd->right == nullptr) {
            err_info ("problems in backend: decrem (3)\n");
            return false;
        }

        push (nd->right);

        write ("\npush 1\nsub\npopm [ax+");
        reg_name (nd->right);
        write ("]\n");

        return true;
    }
    else {
        err_info ("problems in backend: decrem (2)\n");
        return false;
    }
}

bool b_end::out (node *nd) {
    if (nd == nullptr) {
        err_info ("problems in backend: out (1)\n");
        return false;
    }

    if (nd->type == OUT) {
        if (nd->right == nullptr) {
            err_info ("problems in backend: out (3)\n");
            return false;
        }

        push (nd->right);
        write ("out\n");

        return true;
    }
    else {
        err_info ("problems in backend: decrem (2)\n");
        return false;
    }
}

bool b_end::equation (node *nd) {
    if (nd == nullptr) {
        err_info ("problems in backend: out (1)\n");
        return false;
    }

    bool have_problems = false;

    if (nd->is_left ()) {
        have_problems = have_problems || equation (nd->left);
    }
    if (nd->is_right ()) {
        have_problems = have_problems || equation (nd->right);
    }

    switch (nd->type) {
        case ID: {
            push (nd);
            break;
        }
        case NUMBER: {
            push (nd);
            break;
        }
        case PLUS: {
            write ("add\n");
            break;
        }
        case MINUS: {
            write ("sub\n");
            break;
        }
        case MULT: {
            write ("mul\n");
            break;
        }
        case DIVIDE: {
            write ("div\n");
            break;
        }
    }
}

bool backend (node* nd, const char* program_name = "./default/default_program.txt") {
    FILE* pFile = fopen (program_name, "w");

    b_end* nds = new b_end (pFile, nd);
    nds->write ("call reign\nend\n\n");

    bool were_problems = nds->all ();
    fclose (pFile);

    return were_problems;
}

#endif