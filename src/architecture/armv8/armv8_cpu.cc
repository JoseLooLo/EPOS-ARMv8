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
    ASM("str x20, [%0,#16]\n"
    "ldr x20, [sp], #8\n"
    : : "r"(this));

    ASM("str x0, [sp, #-8]!"); //Save x0
    ASM(
    "str x0, [%0,#24]\n"
    "str x1, [%0,#32]!\n"
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
    ".save_pc_temp:\n"
    "adr x20, .save_pc_temp\n"
    //"mov x20, .       \n"
    "str x20, [%0,#8]!\n"
    "ldr x20, [sp], #8\n"
     : : "r"(this));

     ASM("ldr x0, [sp], #8\n"); //Restaure x0
}

void CPU::Context::load() const volatile
{
    ASM("mov sp, %0     \n"
        "isb            \n" : : "r"(this));

    ASM("ldr x20, [sp], #8");
    ASM("msr sp_el0, x20 \n"
        "isb            \n");
    ASM("add sp, sp, #8    \n");       // skip usp, ulr

    //Load flags
    ASM("ldr x20, [sp], #8\n");
    x20_to_state();

    //Load regs
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
        //load PC
        ASM(
        "ldr  x30, [sp], #8             \n"
        "msr elr_el1, x30               \n"
        "ldr  x30, [sp, #-16]           \n");
        //Run the new context
        ASM("eret\n");

}

// This function assumes A[T]PCS (i.e. "o" is in r0/a0 and "n" is in r1/a1)
void CPU::switch_context(Context ** o, Context * n)
{
    //Save context
    ASM(
    //PC and LR
    "str x30, [sp, #-16]\n" //Save LR first then we can use x30 to get the pc
    "adr x30, .ret    \n"   //Get PC
    "str x30, [sp, #-8]\n"   //Save PC
    "add sp, sp, #-16\n"    //Align the stack with the context object
    //x30 to x0
    "str x30, [sp,#-8]!\n"
    "str x29, [sp,#-8]!\n"
    "str x28, [sp,#-8]!\n"
    "str x27, [sp,#-8]!\n"
    "str x26, [sp,#-8]!\n"
    "str x25, [sp,#-8]!\n"
    "str x24, [sp,#-8]!\n"
    "str x23, [sp,#-8]!\n"
    "str x22, [sp,#-8]!\n"
    "str x21, [sp,#-8]!\n"
    "str x20, [sp,#-8]!\n"

    "str x19, [sp,#-8]!\n"
    "str x18, [sp,#-8]!\n"
    "str x17, [sp,#-8]!\n"
    "str x16, [sp,#-8]!\n"
    "str x15, [sp,#-8]!\n"
    "str x14, [sp,#-8]!\n"
    "str x13, [sp,#-8]!\n"
    "str x12, [sp,#-8]!\n"
    "str x11, [sp,#-8]!\n"
    "str x10, [sp,#-8]!\n"

    "str x9, [sp,#-8]!\n"
    "str x8, [sp,#-8]!\n"
    "str x7, [sp,#-8]!\n"
    "str x6, [sp,#-8]!\n"
    "str x5, [sp,#-8]!\n"
    "str x4, [sp,#-8]!\n"
    "str x3, [sp,#-8]!\n"
    "str x2, [sp,#-8]!\n"
    "str x1, [sp,#-8]!\n"
    "str x0, [sp,#-8]!\n"
    );
    state_2_x20();
    ASM(
    "str x20, [sp,#-8]!\n" //Flags
    "add sp, sp, #-8\n" //ulr
    "mrs x20, sp_el0 \n"
    "str x20, [sp,#-8]!\n"
    "mov x20, sp\n"
    "str x20, [x0]\n" // update Context * volatile * o
    );

    ASM("mov sp, x1     \n"
        "isb            \n"); // serialize the pipeline so that SP gets updated before the pop

    ASM("ldr x20, [sp], #8");
    ASM("msr sp_el0, x20 \n"
        "isb            \n");
    ASM("add sp, sp, #8    \n");       // skip ulr

    //Load flags
    ASM("ldr x20, [sp], #8\n");
    x20_to_state();

    //Load regs
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
        //load PC
        ASM(
        "ldr  x30, [sp], #8             \n"
        "msr elr_el1, x30               \n"
        "ldr  x30, [sp, #-16]           \n");
        //Run the new context
        ASM("eret\n");

        //Old context return address
        ASM(".ret: br x30\n");
}

__END_SYS
