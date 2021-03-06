#pragma once

#include <truth/types.h>
#include <truth/memory.h>

struct page_table {
    phys_addr physical_address;
};

enum status checked map_page(void *virtual_address, phys_addr phys_address,
        enum memory_attributes perms);

void unmap_page(void *virtual_address, bool free_physical_memory);

enum status map_external_page(struct page_table *page_table,
        void *virtual_address, phys_addr phys_address, enum memory_attributes
        perms);

void unmap_external_page(struct page_table *page_table, void *virtual_address,
        bool free_physical_memory);



void page_table_switch(phys_addr physical_address);

enum status paging_init(void);

struct page_table *page_table_init(void);

struct page_table *page_table_new_current(void);
struct page_table *page_table_clone(struct page_table *pt);

void page_table_fini(struct page_table *pt);

void debug_paging(void);
