// EPOS ARMv8 CPU Mediator Implementation

#include <architecture/armv8/armv8_cpu.h>

__BEGIN_SYS

// Class attributes
unsigned int CPU::_cpu_clock;
unsigned int CPU::_bus_clock;

// Class methods
void CPU::Context::save() volatile
{
    ASM(
    "str x0, [%0,#24]!\n"
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

    //pc
    "str x20, [sp, #-8]!\n"
    "mov x20, .         \n"
    "str x20, [%0,#8]!\n"
    "ldr x20, [sp], #8\n"
     : : "r"(this));
//     ASM(
//         "       str     r12, [sp,#-68]          \n"
//         "       mov     r12, pc                 \n");
// if(thumb)
//     ASM("       orr     r12, #1                 \n");

//     ASM("       str     r12, [%0, #-64]         \n"
//         "       ldr     r12, [sp, #-68]         \n"
//         "       str     lr,  [%0, #-60]         \n"
//         "       str     r12, [%0, #-56]         \n"
//         "       str     r11, [%0, #-52]         \n"
//         "       str     r10, [%0, #-48]         \n"
//         "       str     r9,  [%0, #-44]         \n"
//         "       str     r8,  [%0, #-40]         \n"
//         "       str     r7,  [%0, #-36]         \n"
//         "       str     r6,  [%0, #-32]         \n"
//         "       str     r5,  [%0, #-28]         \n"
//         "       str     r4,  [%0, #-24]         \n"
//         "       str     r3,  [%0, #-20]         \n"
//         "       str     r2,  [%0, #-16]         \n"
//         "       str     r1,  [%0, #-12]         \n"
//         "       str     r0,  [%0, #-8]          \n" : : "r"(this));
//     psr_to_r12();
//     ASM("       str     r12, [%0, #-4]          \n"
//         "       ldr     r12, [sp, #-68]         \n"
//         "       add     %0, %0, #-68            \n"
//         "       bx      lr                      \n" : : "r"(this));
}

void CPU::Context::load() const volatile
{
    // ASM("       mov     sp, %0                  \n"
    //     "       isb                             \n" : : "r"(this)); // serialize the pipeline so that SP gets updated before the pop

    // ASM("       add     sp, #8                  \n");       // skip usp, ulr

    // ASM("       pop     {r12}                   \n");
    // r12_to_psr();                                           // the context is loaded in SVC; with multitasking, a second context drives a mode change at _int_leave
    // ASM("       pop     {r0-r12, lr}            \n"
    //     "       pop     {pc}                    \n");
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
