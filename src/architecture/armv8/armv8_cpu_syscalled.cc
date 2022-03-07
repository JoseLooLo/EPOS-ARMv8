// EPOS RISC-V 32 System Call Entry Implementation

#include <architecture/armv8/armv8_cpu.h>

extern "C" { void _exec(void *); }

__BEGIN_SYS

void CPU::syscalled(void * msg) {
    // We get here when an APP triggers INT_SYSCALL (i.e. ecall)
    // Enable interruptions
    CPU::int_enable();
    if(Traits<Build>::MODE == Traits<Build>::KERNEL) {
        _exec(reinterpret_cast<void *>(msg)); // the message to EPOS Framework is passed on register a1
        // CPU::a0(sizeof(void *));                    // tell IC::entry to perform PC = PC + 4 on return
    }
}
__END_SYS
