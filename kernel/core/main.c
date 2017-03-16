#include <truth/panic.h>
#include <truth/cpu.h>
#include <truth/types.h>
#include <truth/log.h>
#include <truth/slab.h>
#include <truth/heap.h>
#include <truth/memory.h>
#include <truth/physical_allocator.h>
#include <truth/process.h>
#include <truth/timer.h>
#include <arch/x64/paging.h>


const char *Logo = "\n"
                   "            _.-.\n"
                   "        .-.  `) |  .-.\n"
                   "    _.'`. .~./  \\.~. .`'._\n"
                   " .-' .'.'.'.-|  |-.'.'.'. '-.\n"
                   "  `'`'`'`'`  \\  /  `'`'`'`'`\n"
                   "             /||\\\n"
                   "            //||\\\\\n"
                   "\n"
                   "      The Kernel of Truth\n";


void kernel_main(void *multiboot_tables) {
    assert_ok(log_init(Log_Debug, "log"));
    log(Log_None, Logo);
    logf(Log_None, "\tVersion %d.%d.%d\n\tCommit %s\n\t%s\n\tCPU Time %ld\n",
         kernel_major, kernel_minor, kernel_patch, vcs_version,
         project_website, cpu_time());
    interrupts_init();
    physical_allocator_init(multiboot_tables);
    slab_init();
    assert_ok(heap_init());
    memory_init();
    assert_ok(paging_init());
    assert_ok(processes_init());
    timer_init();
    halt();
}
