//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//!    Exit codes for cpu, asm, disasm
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

#ifndef EXIT_CODES_ON
#define EXIT_CODES_ON

enum exits {
    all_right,
    unknown_command,
    label_twice,
    func_twice,
    push_problem,
    non_exist_elem,
    div_zero,
    no_args,
    not_ram,
};

const char* exits_expl[10] = {
        "All_right, men.",
        "Unknown command, rewrite your code. Search ready-to-use commands in commands.h\n",
        "You have used label declaration twice!\n",
        "You have used func declaration twice!\n",
        "You have some problems with push. Maybe you can search your mistake in stack_dump.txt\n",
        "You are trying to communicate with non-existing element. Go to psychologist.\n",
        "Even people in first form know that it is forbidden to divide by zero. Unfortunately, you don't...\n",
        "You didn't put enough arguments somewhere in your code.\n",
        "We are just a startup now, so we don't have enough RAM now.\n"
};

#endif