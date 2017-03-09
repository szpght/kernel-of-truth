#pragma once

#define KB 0x400
#define MB 0x100000
#define GB 0x40000000

#ifdef __ASM__
#define Page_Small  (4 * KB)
#define Page_Medium (2 * MB)
#define Page_Large  (GB)

#define Memory_Present       (1 << 0)
#define Memory_Writable      (1 << 1)
#define Memory_User_Access   (1 << 2)
#define Memory_No_Execute    (1 << 63)
#define Memory_Execute_Mask  (0x7fffffffffffffff)
#define Memory_Writable_Mask (0xfffffffffffffffd)


#elif __C__
enum page_size {
    Page_Small  = (4 * KB),
    Page_Medium = (2 * MB),
    Page_Large  = (GB),
};

enum memory_attributes {
    Memory_No_Attributes = (0),
    Memory_Present       = (1ul << 0),
    Memory_Writable      = (1ul << 1),
    Memory_User_Access   = (1ul << 2),
    Memory_No_Execute    = (1ul << 63),
};

void memory_init(void);
#endif
