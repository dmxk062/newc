#pragma once

enum MapProt {
    Prot_Read = 1,
    Prot_Write = 2,
    Prot_Exec = 4,
    Prot_None = 0,
};

enum MapFlag {
    Map_Shared = 1,
    Map_Private = 2,
    Map_Anonymous = 32,
};

Result(void_p) mmap(untyped addr, u64 len, enum MapProt, enum MapFlag flags, u64 fd, u64 offset);
errno mmunmap(untyped addr, u64 len);
