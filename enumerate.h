//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!
//!    Makes operating from code to name of command and vice versa more easy and global
//!    Name, code number, size in code (in bytes, with arguments), code to insert in CPU
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

#ifndef INCLUDE_ENUMERATE
#define INCLUDE_ENUMERATE

#define DEF_CMD(name, number, bytes, cpu_code, asm_code) kdr_##name = number,

enum command_c {
    #include "commands.h"
};

#undef DEF_CMD

#endif