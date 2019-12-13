#include "tree.cpp"
#include "asm.cpp"
#include "CPU.cpp"
#include "backend.h"

int main() {
    //diff_test ();

    node* nd = rec_descent ();
    nd->photo ();

    front_reverse (nd);
    backend (nd);

    assembler ();
    processor ();

    return 0;
}