#include "mprotect-diffckpt.h"

addr_t PAGE_SIZE;
addr_t PAGE_MASK;
const int UINT64 = sizeof(uint64_t); 
struct sigaction sa;
struct sigaction sa_old;
prot_vec prot_data;
d_page_vec dirty_pages;

void init() {
    // get page mask
    PAGE_SIZE = (addr_t) sysconf(_SC_PAGESIZE);
    PAGE_MASK = ~((addr_t)0x0);
    addr_t tail = (addr_t)0x1;
    for(; tail!=PAGE_SIZE; PAGE_MASK<<=1, tail<<=1);
    // register signal handler
    sa.sa_flags = SA_SIGINFO|SA_NODEFER;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    sigaction(SIGSEGV, &sa, &sa_old);
    prot_data.prot_vars = NULL;
    prot_data.count = 0;
    dirty_pages.addr_vec = (addr_pt*) NULL;
    dirty_pages.count = 0;
}

size_t add_prot(addr_pt data, size_t size) {
    prot_data.prot_vars = realloc(prot_data.prot_vars, (++(prot_data.count))*sizeof(prot_t));
    prot_t *elem = &prot_data.prot_vars[prot_data.count-1];
    elem->addr = data;
    elem->addr_val = (addr_t) data;
    elem->size = size;
    elem->id = prot_data.count;
    elem->dirty = false;
    // determine pages which belong fully to data set
    addr_t first_page = (addr_t) get_first_incl_page(data);
    addr_t last_page = (addr_t) get_last_incl_page(data+size);
    addr_t psize = 0;
    if (first_page < last_page) {
        psize = last_page - first_page + PAGE_SIZE;
        mprotect(get_first_incl_page(data), psize, PROT_READ);
    } else {
        first_page = (addr_t) NULL;
        last_page = (addr_t) NULL;
    }
    // debug information
    printf("ID: %lu, size: %lu, pages protect: %lu, addr: %p, first page: %p, last page: %p\n", 
            elem->id, 
            elem->size, 
            psize/PAGE_SIZE,
            data,
            (addr_pt) first_page,
            (addr_pt) last_page);
    // return id of protected variable
    return elem->id;
}

addr_pt get_first_incl_page(addr_pt addr_ptr) {
    addr_pt page; 
    addr_t page_val; 
    addr_t addr_val = (addr_t) addr_ptr;
    page_val = (addr_val+PAGE_SIZE-1) & PAGE_MASK;
    page = (addr_pt) page_val;
    return page;
}

addr_pt get_last_incl_page(addr_pt addr_ptr) {
    addr_pt page; 
    addr_t page_val; 
    addr_t addr_val = (addr_t) addr_ptr;
    page_val = (addr_val-PAGE_SIZE+1) & PAGE_MASK;
    page = (addr_pt) page_val;
    return page;
}

bool is_valid_request(addr_t addr_val) {
    bool ge, le;
    for(int i=0; i<prot_data.count; ++i) {
        ge = addr_val >= prot_data.prot_vars[i].addr_val;
        le = addr_val <= (prot_data.prot_vars[i].addr_val + prot_data.prot_vars[i].size);
        if (ge && le) {
            return true;
        }
    }
    return false;
}

void handler(int signum, siginfo_t* info, void* ucontext) {
    if (signum == SIGSEGV) {
        if(is_valid_request((addr_t)info->si_addr)){
            printf("SIGSEGV signal was raised at address 0x%" PRIxPTR "\n", (addr_t) info->si_addr);
            mprotect((addr_pt)((addr_t)info->si_addr & PAGE_MASK), PAGE_SIZE, PROT_READ|PROT_WRITE);
            dirty_pages.addr_vec = (addr_pt*) realloc(dirty_pages.addr_vec, (++(dirty_pages.count))*sizeof(addr_pt));
            dirty_pages.addr_vec[dirty_pages.count - 1] = (addr_pt)((addr_t)info->si_addr & PAGE_MASK);
        } else {
            sigaction(SIGSEGV, &sa_old, NULL);
            raise(SIGSEGV);
            sigaction(SIGSEGV, &sa, &sa_old);
        }
    }
}

void finalize() {
    printf("dirty pages: \n");
    for(int i=0; i<dirty_pages.count; ++i) {
        printf("[%d] %p\n", i, dirty_pages.addr_vec[i]);
    }
    for(int i=0; i<prot_data.count; ++i) {
        addr_t first_page = (addr_t) get_first_incl_page(prot_data.prot_vars[i].addr);
        addr_t last_page = (addr_t) get_last_incl_page(prot_data.prot_vars[i].addr+prot_data.prot_vars[i].size);
        addr_t psize = 0;
        if (first_page < last_page) {
            psize = last_page - first_page + PAGE_SIZE;
            mprotect((addr_pt) first_page, psize, PROT_READ|PROT_WRITE);
        } else {
            first_page = (addr_t) NULL;
            last_page = (addr_t) NULL;
        }
    }
    free(prot_data.prot_vars);
    free(dirty_pages.addr_vec);
}
