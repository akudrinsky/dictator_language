#ifndef FRONTEND_REV
#define FRONTEND_REV

#include "tree.h"

#define flag printf ("\033[32;1m flag \033[0m\n\n");

#define error(code) fprintf(for_errors,"front_rev::%s (%s)\n", __PRETTY_FUNCTION__, #code);\
                    return false;

struct front_rev {
    node* root;
    FILE* dist;
    FILE* for_errors;

    explicit front_rev (node* nodes = nullptr, const char* filename = "program_reversed.txt");

    bool global (node* nd);
    bool def_func (node* nd);
    bool arguments (node* nd);
    bool variable (node* nd);

    bool get_id (node* nd);

    bool instruction (node* nd);
    bool get_number (node* nd);

    bool condition (node* nd);
    bool cycle (node* nd);

    bool print (node* nd);

    bool equation (node* nd);
    bool multiplication (node* nd);
    bool brackets (node* nd);
};

front_rev::front_rev(node* nodes, const char* filename) {
    root = nodes;
    dist = fopen (filename, "w");
    for_errors = fopen ("errors_in_front_rev.txt", "w");
    if (dist == nullptr) {
        err_info ("front_rev::front_rev (1)\n");
    }
}

bool front_rev::global(node* nd) {
    bool all_right = true;

    if (nd->type != BLOCK) {
        err_info ("front_rev::global (1)\n");
        return false;
    }

    if (nd->left != nullptr) {
        all_right = all_right && global (nd);
    }

    if (nd->right == nullptr) {
        err_info ("front_rev::global (2)\n");
    }

    if (nd->right->type == DEFINITION) {
        all_right = all_right && def_func (nd->right);
    }
    else if (nd->right->type == VARIABLE) {
        all_right = all_right && variable (nd->right);
    }
    else {
        err_info ("front_rev::global (3)\n");
    }

    return all_right;
}

bool front_rev::def_func(node *nd) {
    bool all_right = true;

    if (nd->type != DEFINITION) {
        error (1)
    }



    return all_right;
}

bool front_reverse (node* nd) {
    front_rev* rev = new front_rev (nd);

    bool all_right = true;
    all_right = all_right && rev->global (nd);

    return all_right;
}

#endif