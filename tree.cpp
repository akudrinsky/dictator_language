//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! It is my implementation of wolphram alpha derivatives
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

#ifndef TREE_WAS_DEFINED                           //flag that tree was defined only once in all project
#define TREE_WAS_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tree.h"
#include "my_lyb.h"
#include "operations.h"
#include <new>

#define DEBUG
#ifdef DEBUG
#define ASSERT(cond)\
    if (!(cond)) {\
        printf ("Assert failure: < %s > in file %s, line %d, function %s\n", #cond, __FILE__, __LINE__, __PRETTY_FUNCTION__);\
        abort();\
}
#else
#define ASSERT(cond) ;
#endif

#define el_array(name)\
sizeof (name) / sizeof (name[0])

#define get_rand_elem(name)\
name[rand() % (sizeof (name) / sizeof (name[0]))]

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Kind of analog of node construct, but it is more convinient for recursive diff
//!
//! @param [in] data - value
//! @param [in] type - type of node
//! @param [in] left - left leaf
//! @param [in] right - right leaf
//!
//! @return pointer to newly created node
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
node* create_node (char* data, char type, node* left, node* right) {
    node* nd = new node;
    ASSERT (nd != nullptr)

    nd->data = data;
    nd->type = type;
    nd->left = left;
    nd->right = right;

    if (nd->is_left())
        nd->left->parent = nd;
    if (nd->is_right())
        nd->right->parent = nd;

    ASSERT (nd != nullptr)

    return nd;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Node construct
//!
//! @param [in] type - type of node
//! @param [in] left - left leaf
//! @param [in] right - right leaf
//! @param [in] prnt - parent node
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
node::node(const char tp, char* str, node* left_l, node* right_l, node* prnt) {
    data = str;
    left = left_l;
    right = right_l;
    parent = prnt;
    type = tp;                                         //unknown
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! Destructor of a tree
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
node::~node() {                         //make it!!!

}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! Safe clearing of a tree
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void node::clear() {
    if (is_left()) {
        left->clear();
        delete left;
    }
    if (is_right()) {
        right->clear();
        delete right;
    }

    left = nullptr;
    right = nullptr;
    parent = nullptr;
    type = UNKNOWN;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Merging leaf to a given node from a given side
//!
//! @param [in] where - l for left, r for right
//! @param [in] leaf - leaf to merge
//! @return - if it was OK
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::merge(node* leaf, char where) {
    if (leaf == nullptr) {
        if (where == 'l') {
            left = nullptr;
        }
        else if (where == 'r') {
            right = nullptr;
        }
        else {
            err_info ("Wrong format of merge, where = ");
            err_info (&where);
            err_info ("\n");
            return false;
        }
    }
    else {
        //printf ("son in merge: \t%p\n", leaf);
        if (where == 'l') {
            left = leaf;
            leaf->parent = this;
        }
        else if (where == 'r') {
            right = leaf;
            leaf->parent = this;
        }
        else {
            err_info ("Wrong format of merge, where = ");
            err_info (&where);
            err_info ("\n");
            return false;
        }
    }

    return true;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Makes picture of a tree
//!
//! @param [in] pict_name - name of photo
//! @param [in] pict_type - type (ex: png)
//! @param [in] iter - NOT FOR USERS
//! @param [in] pFile - NOT FOR USERS
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void node::photo(const char* pict_name, const char* pict_type, int iter, FILE* pFile) {
    ASSERT (pict_name != nullptr)
    ASSERT (pict_type != nullptr)

    if (iter == 1) {
        pFile = fopen ("tree_graph.dot", "w");
        ASSERT (pFile != nullptr)
        fprintf (pFile, "digraph G{\n\tedge[color=\"chartreuse4\",fontcolor=\"blue\",fontsize=12];\n\tnode[shape=\"rectangle\",fontsize=15];\n");
    }
    ASSERT (pFile != nullptr)

    //vertices
    /*
    if (type == UNKNOWN) {
        fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| {unknown | %s} | <f1> %p\" ];\n", iter, left, data, right);
    }
    else if (type == NUMBER) {
        fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| {%s | num} | <f1> %p\" ];\n", iter, left, data, right);
    }
    else if (type == VARIABLE) {
        fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| {var | %s} | <f1> %p\" ];\n", iter, left, data, right);
    }
    else {
        fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| {%s | op} | <f1> %p\" ];\n", iter, left, tokens[type].name, right);
    }//3 - look in operations.h
     */
    fprintf (pFile, "\t%d [shape=record,label=\"  <f0> %p| {%s | %s} | <f1> %p\" ];\n", iter, left, tokens[type].name, data, right);

    //edges
    if (left != nullptr) {
        (*left).photo (pict_name, pict_type, iter * 2, pFile);
        fprintf (pFile, "\t\t%d:<f0> -> %d\n", iter, iter * 2);
    }
    if (right != nullptr) {
        (*right).photo (pict_name, pict_type, iter * 2 + 1, pFile);
        fprintf (pFile, "\t\t%d:<f1> -> %d[color=\"orange\"]\n", iter, iter * 2 + 1);
    }

    if (iter == 1) {
        fprintf (pFile, "}");
        fclose (pFile);

        //executing command in terminal
        const int max_cmd_size = 50;
        char command[max_cmd_size] = "dot tree_graph.dot -T ";
        strcat (command, pict_type);
        strcat (command, " -o ");
        strcat (command, pict_name);

        system (command);
    }
}
//#include "rec.h"
#include "tokenizer.h"
#include "frontend.h"

/*
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! Saves tree to a txt file format
//!
//! @param [in] filename - name of file to save tree to
//! @param [in] mode - 'a' for prefix, 'd' for infix
//! @param [in] pFile - NOT FOR USERS
//! @param [in] is_first - NOT FOR USERS
//! @param [in] need_closing - NOT FOR USERS
//! @return - if it was OK
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::save(const char* filename, FILE* pFile, bool is_first, bool need_closing) {
    ASSERT (filename != nullptr)

    bool status = true;
    if (is_first) {
        pFile = fopen (filename, "w");
        ASSERT (pFile != nullptr)
    }

    fprintf (pFile, "(");

    if (left == nullptr && right == nullptr) {
        if (type == number)
            fprintf (pFile, "%lf", data);
        else if (type == variable)
            fprintf (pFile, "x");
        else
            fprintf (pFile, "%s", operations[type]);
    }
    if (left == nullptr && right != nullptr) {
        fprintf (pFile, "@");

        if (type == number)
            fprintf (pFile, "%lf", data);
        else if (type == variable)
            fprintf (pFile, "x");
        else
            fprintf (pFile, "%s", operations[type]);

        if (!(*right).save(filename, pFile, false, false))
            status = false;
    }
    if (left != nullptr && right == nullptr) {
        if (!(*left).save (filename, pFile, false, false))
            status = false;

        if (type == number)
            fprintf (pFile, "%lf", data);
        else if (type == variable)
            fprintf (pFile, "x");
        else
            fprintf (pFile, "%s", operations[type]);

        fprintf (pFile, "@");
    }
    if (left != nullptr && right != nullptr) {
        if (!(*left).save (filename, pFile, false, false))
            status = false;

        if (type == number)
            fprintf (pFile, "%lf", data);
        else if (type == variable)
            fprintf (pFile, "x");
        else
            fprintf (pFile, "%s", operations[type]);

        if (!(*right).save (filename, pFile, false, true))
            status = false;
    }

    fprintf (pFile, ")");

    if (is_first) {
        fclose (pFile);
    }

    return status;
}
 */

node* rec_descent(const char *filename) {
    FILE* input = fopen (filename, "r");

    char* str = read_text (input);
    //printf ("%s", str);

    cell *nodes = tokenize (str);


    printf ("tokens:\n");
    int i = 0;
    while (nodes[i].type != UNKNOWN) {
        printf ("%9s\t\t%s\n", tokens[nodes[i].type].name, nodes[i].name);
        ++i;
    }

    frontend* parser = new frontend (nodes);


    return parser->all ();
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! If a node has left child
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::is_left() {
    return left != nullptr;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//! If a node has right child
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::is_right() {
    return right != nullptr;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//! If a tree is valid
//!
//! @return if it was OK
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
bool node::is_valid() {
    //printf ("valiator launched\n");
    if (is_left()) {
        if (this != this->left->parent) {
            err_info ("Lost connection between node and his left child\n");
        }
        if (!this->left->is_valid()) {
            return false;
        }
    }

    if (is_right()) {
        if (this != this->right->parent) {
            err_info ("Lost connection between node and his right child\n");
            return false;
        }
        if (!this->right->is_valid()) {
            return false;
        }
    }

    return true;
}

#endif