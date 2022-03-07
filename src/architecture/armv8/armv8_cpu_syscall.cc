// EPOS ARMv8 CPU System Call Function Implementation

#include <architecture/armv8/armv8_cpu.h>
#include <machine/ic.h>

__BEGIN_SYS

void CPU::syscall(void * msg)
{
    ASM("SVC #80"); //msg is on x0
    // ASM("int %0" : : "i"(IC::INT_SYSCALL), "c"(msg));
}

__END_SYS
