#ifndef MPROTECT_DIFFCKPT_H
#define MPROTECT_DIFFCKPT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

typedef long sysinfo_t;     // return values of sysconf calls
typedef char* mempool_pt;    // type for memoryallocations

typedef void* addr_pt;        // address value
typedef uintptr_t addr_t;

// protected variable struct
typedef struct prot_t {
    addr_pt addr;
    addr_t addr_val;
    bool dirty;
    size_t size;
    size_t id;
} prot_t;

// protected variables container
typedef struct prot_vec {
    prot_t * prot_vars;
    size_t count;
} prot_vec;

// dirty pages container
typedef struct d_page_vec {
    addr_pt* addr_vec;
    size_t count;
} d_page_vec;

extern addr_t PAGE_SIZE;
extern addr_t PAGE_MASK;

extern const int UINT64;
extern struct sigaction sa;

extern prot_vec prot_data;
extern d_page_vec dirty_pages;

void init();
addr_pt get_first_incl_page(addr_pt addr);
addr_pt get_last_incl_page(addr_pt addr);
addr_t get_page_offset(addr_pt addr_ptr);
void handler(int signum, siginfo_t* info, void* ucontext);
addr_pt get_prot_addr();
size_t add_prot(addr_pt data, size_t size);
bool is_valid_request(addr_t addr_val);
void finalize();

#endif // MPROTECT_DIFFCKPT_H
