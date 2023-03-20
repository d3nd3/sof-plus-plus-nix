#include "common.h"

#include <unistd.h>
#include <sys/mman.h>

void error(const char* message) {
    std::cerr << "Error: " << message << std::endl;
    std::exit(EXIT_FAILURE);
}

void hexdump(void *addr_start, void *addr_end) {
    unsigned char *p = (unsigned char *)addr_start;
    unsigned char *q = (unsigned char *)addr_end;
    int offset = 0;
    while (p < q) {
        printf("%08x: ", offset);
        for (int i = 0; i < 16 && p + i < q; i++) {
            printf("%02x ", *(p + i));
        }
        for (int i = 16 - (q - p); i > 0; i--) {
            printf("   ");
        }
        for (int i = 0; i < 16 && p + i < q; i++) {
            printf("%c", *(p + i) >= 0x20 && *(p + i) < 0x7f ? *(p + i) : '.');
        }
        printf("\n");
        p += 16;
        offset += 16;
    }
}

void memoryAdjust(void * addr, int size, unsigned char val)
{
    memoryUnprotect(addr);
    memset(addr,val,size);
    memoryProtect(addr);
}

void memoryUnprotect(void * addr)
{
    size_t page_size = getpagesize();

    void * aligned_addr = (void *)((unsigned long)addr & ~(page_size - 1));
    if (mprotect(aligned_addr, page_size, PROT_READ | PROT_WRITE) == -1) {
        error("mprotect");
    }
}

void memoryProtect(void * addr)
{
    size_t page_size = getpagesize();
    void * aligned_addr = (void *)((unsigned long)addr & ~(page_size - 1));
    if (mprotect(aligned_addr, page_size, PROT_READ ) == -1) {
        error("mprotect");
    }
}


void callE8Patch(void * addr,void * to) {

    memoryUnprotect(addr);

    unsigned char * p = addr;
    p[0] = 0xE8;
    *(int*)(p+1) = to - (int)addr - 5;

    memoryProtect(addr);
}

void * createDetour(void * orig, void * mine, int size) {
    memoryUnprotect(orig);

    /*
        THUNK SAVED. { SAVED OPCODES;JMP->ORIG+SIZE(SAVED_OPCODES) }
    */
    void * thunk = malloc(size + 5);
    // save the front of func
    memcpy(thunk, orig, size);
    // jump back
    unsigned char * p = thunk + size;
    *p = 0xE9;
    int rel_jmp = ((int)orig+size) - (int)p - 5;
    memcpy(p+1,&rel_jmp,4);

    /*
        PATCH 5 bytes start of orig
    */
    unsigned char small_jmp;
    // write the front of func
    small_jmp = 0xE9;
    rel_jmp = mine - (int)orig - 5;
    memcpy(orig,&small_jmp,1);
    memcpy(orig+1,&rel_jmp,4);

    memoryUnprotect(orig);
    // dealloc this on uninit.
    return thunk;
}