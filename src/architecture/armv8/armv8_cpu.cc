// EPOS ARMv8 CPU Mediator Implementation

#include <architecture/armv8/armv8_cpu.h>

__BEGIN_SYS

// Class attributes
unsigned int CPU::_cpu_clock;
unsigned int CPU::_bus_clock;

// Class methods
void CPU::Context::save() volatile
{
    ASM("str x20, [sp, #-8]!\n");
    state_2_x20();
    ASM("str x20, [%0,#16]!\n"
    "ldr x20, [sp], #8\n"
    : : "r"(this));

    ASM(
    "str x0, [%0,#8]!\n"
    "str x1, [%0,#8]!\n"
    "str x2, [%0,#8]!\n"
    "str x3, [%0,#8]!\n"
    "str x4, [%0,#8]!\n"
    "str x5, [%0,#8]!\n"
    "str x6, [%0,#8]!\n"
    "str x7, [%0,#8]!\n"
    "str x8, [%0,#8]!\n"
    "str x9, [%0,#8]!\n"
    "str x10, [%0,#8]!\n"

    "str x11, [%0,#8]!\n"
    "str x12, [%0,#8]!\n"
    "str x13, [%0,#8]!\n"
    "str x14, [%0,#8]!\n"
    "str x15, [%0,#8]!\n"
    "str x16, [%0,#8]!\n"
    "str x17, [%0,#8]!\n"
    "str x18, [%0,#8]!\n"
    "str x19, [%0,#8]!\n"
    "str x20, [%0,#8]!\n"

    "str x21, [%0,#8]!\n"
    "str x22, [%0,#8]!\n"
    "str x23, [%0,#8]!\n"
    "str x24, [%0,#8]!\n"
    "str x25, [%0,#8]!\n"
    "str x26, [%0,#8]!\n"
    "str x27, [%0,#8]!\n"
    "str x28, [%0,#8]!\n"
    "str x29, [%0,#8]!\n"
    "str x30, [%0,#8]!\n"
    //lr
    "str x30, [%0,#8]!\n"
    : : "r"(this));
    //TODO pc
    ASM(
    "str x20, [sp, #-8]!\n"
    "mov x20, .         \n"
    "str x20, [%0,#8]!\n"
    "ldr x20, [sp], #8\n"
     : : "r"(this));
}

void CPU::Context::load() const volatile
{
    ASM("mov sp, %0     \n"
        "isb            \n" : : "r"(this)); // serialize the pipeline so that SP gets updated before the pop

    ASM("add sp, sp, #16    \n");       // skip usp, ulr

    //Load flags
    ASM("ldr x20, [sp], #8\n");
    // x20_to_state();

    ASM("str x20, [sp, #-8]!\n");
    state_2_x20();
    x20_to_state();
    ASM(
    "ldr x20, [sp]\n"
    "add sp, sp, 8\n"
    : : "r"(this));

    //Load regs
    //This is probably a bug
    ASM("ldr  x0, [sp], #8              \n"
		"ldp	x1, x2, [sp], #16       \n"
		"ldp	x3, x4, [sp], #16       \n"
		"ldp	x5, x6, [sp], #16       \n"
		"ldp	x7, x8, [sp], #16       \n"
		"ldp	x9, x10, [sp], #16      \n"
		"ldp	x11, x12, [sp], #16     \n"
		"ldp	x13, x14, [sp], #16     \n"
		"ldp	x15, x16, [sp], #16     \n"
		"ldp	x17, x18, [sp], #16     \n"
		"ldp	x19, x20, [sp], #16     \n"
		"ldp	x21, x22, [sp], #16     \n"
		"ldp	x23, x24, [sp], #16     \n"
		"ldp	x25, x26, [sp], #16     \n"
		"ldp	x27, x28, [sp], #16     \n"
		"ldp	x29, x30, [sp], #16     \n"
        //lr
        "ldr  x30, [sp], #8             \n");
        //load PC ?
        ASM(
        "ldr  x30, [sp], #8             \n"
        "msr elr_el1, x30               \n"
        "ldr  x30, [sp, #-16]           \n"
        "eret");
}

// This function assumes A[T]PCS (i.e. "o" is in r0/a0 and "n" is in r1/a1)
void CPU::switch_context(Context ** o, Context * n)
{
    ASM("switch_context:");
    // Push the context into the stack and adjust "o" to match
//     ASM("       sub     sp, #4                  \n"     // reserve room for PC
//         "       push    {r0-r12, lr}            \n"     // push all registers (LR first, r0 last)
//         "       adr     r12, .ret               \n");   // calculate the return address using the saved r12 as a temporary
// if(thumb)
//     ASM("       orr     r12, #1                 \n");   // adjust the return address in thumb mode

//     ASM("       str     r12, [sp, #56]          \n");   // save calculated PC

//     if(Traits<FPU>::enabled && !Traits<FPU>::user_save)
//         fpu_save();

//     psr_to_r12();                                       // save PSR to temporary register r12
//     ASM("       push    {r12}                   \n");   // save PSR

//     ASM("       sub     sp, #8                  \n");   // skip ulr and usp

//     ASM("       str     sp, [r0]                \n");   // update Context * volatile * o

//     // Set the stack pointer to "n" and pop the context
//     ASM("       mov     sp, r1                  \n"     // get Context * volatile n into SP
//         "       isb                             \n");   // serialize the pipeline so SP gets updated before the pop

//     ASM("       add     sp, #8                  \n");   // skip usp and ulr

//     ASM("       pop     {r12}                   \n");   // pop PSR into temporary register r12
//     r12_to_psr();                                       // restore PSR

//     if(Traits<FPU>::enabled && !Traits<FPU>::user_save)
//         fpu_restore();

//     ASM("       pop     {r0-r12, lr}            \n");   // pop all registers (r0 first, LR last)

// #ifdef __cortex_m__
//     int_enable();
// #endif

//     ASM("       pop     {pc}                    \n"     // restore PC
//         ".ret:  bx      lr                      \n");   // return
}

__END_SYS
