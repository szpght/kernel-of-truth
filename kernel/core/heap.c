#include <truth/heap.h>
#include <truth/memory.h>
#include <truth/panic.h>
#include <truth/region_vector.h>
#include <truth/slab.h>
#include <truth/string.h>
#include <truth/types.h>

#define Heap_Red_Zone_Size sizeof(unsigned long)
#define Heap_Red_Zone_Fill 0xccccccccccccccccul

#define Heap_Size (16 * Page_Small)

struct region_vector *heap_metadata_used;
struct region_vector *heap_metadata_free;
uint8_t *heap = NULL;

size_t Used;

size_t heap_get_usage(void) {
    return Used;
}

enum status checked heap_init(void) {
    heap_metadata_used = slab_alloc(Page_Small, Memory_Writable);
    if (heap_metadata_used == NULL) {
        assert(0);
        return Error_No_Memory;
    }
    heap_metadata_free = slab_alloc(Page_Small, Memory_Writable);
    if (heap_metadata_free == NULL) {
        slab_free(Page_Small, heap_metadata_used);
        assert(0);
        return Error_No_Memory;
    }
    heap = slab_alloc(Heap_Size, Memory_Writable | Memory_No_Execute);
    if (heap == NULL) {
        slab_free(Page_Small, heap_metadata_used);
        slab_free(Page_Small, heap_metadata_free);
        assert(0);
        return Error_No_Memory;
    }

    region_vector_init(heap_metadata_used);
    region_vector_init(heap_metadata_free);
    region_free(heap_metadata_free, (union address)heap, Heap_Size);

    memset(heap, (int)Heap_Red_Zone_Fill, Heap_Size);

    logf(Log_Info, "Heap rooted at 0x%p with size 0x%x\n", heap, Heap_Size);
    return Ok;
}

void *kmalloc(size_t bytes) {
    union address address;
    size_t allocation_size = bytes + 2 * Heap_Red_Zone_Size;

    enum status status = region_alloc(heap_metadata_free,
                                      allocation_size, &address);
    if (status != Ok) {
        return NULL;
    }
    region_free(heap_metadata_used, address, allocation_size);

    for (size_t i = 0; i < allocation_size / sizeof(Heap_Red_Zone_Fill); ++i) {
        assert(address.ulong[i] == Heap_Red_Zone_Fill);
    }

    assert(address.bytes < heap + Heap_Size);
    assert(address.bytes >= heap);
    assert(address.bytes + bytes > heap);
    assert(address.bytes + bytes <= heap + Heap_Size);
    Used += allocation_size;
    return address.bytes + Heap_Red_Zone_Size;
}

void *krealloc(void *ptr, size_t size) {
    uint8_t *old_mem = ptr;
    if (ptr == NULL) {
        return NULL;
    }
    uint8_t *new_mem = kmalloc(size);
    if (new_mem == NULL) {
        return NULL;
    }
    memcpy(new_mem, old_mem, size);
    return new_mem;
}

void *kcalloc(size_t count, size_t size) {
    void *address = kmalloc(count * size);
    if (address == NULL) {
        return NULL;
    }
    memset(address, 0, count * size);
    return address;
}

void kfree(void *address) {
    if (address == NULL) {
        return;
    }
    union address addr;
    addr.virtual = address - Heap_Red_Zone_Size;
    size_t size = region_find_size_and_free(heap_metadata_used, addr);
    unsigned long *redzone_prefix = addr.virtual;
    unsigned long *redzone_suffix = addr.virtual + size - Heap_Red_Zone_Size;
    assert(*redzone_prefix == Heap_Red_Zone_Fill);
    assert(*redzone_suffix == Heap_Red_Zone_Fill);
    assert(addr.bytes >= heap);
    assert(addr.bytes < heap + Heap_Size);
    assert(addr.bytes + size > heap);
    assert(addr.bytes + size <= heap + Heap_Size);

    memset(heap, (int)Heap_Red_Zone_Fill, Page_Small);
    Used -= size;
    region_free(heap_metadata_free, addr, size);
}
