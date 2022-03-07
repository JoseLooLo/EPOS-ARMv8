// EPOS ARM Cortex IC Mediator Implementation

#include <machine/machine.h>
#include <machine/ic.h>
#include <machine/timer.h>
#include <process.h>

#if defined(__armv8_h)
extern "C" { void _int_entry() __attribute__ ((nothrow, alias("_ZN4EPOS1S2IC5entryEv"))); }
#else
extern "C" { void _int_entry() __attribute__ ((naked, nothrow, alias("_ZN4EPOS1S2IC5entryEv"))); }
#endif
extern "C" { void _dispatch(unsigned int) __attribute__ ((alias("_ZN4EPOS1S2IC8dispatchEj"))); }
extern "C" { void _eoi(unsigned int) __attribute__ ((alias("_ZN4EPOS1S2IC3eoiEj"))); }
extern "C" { void _serror_exception() __attribute__ ((alias("_ZN4EPOS1S2IC21undefined_instructionEv"))); }
extern "C" { void _software_interrupt() __attribute__ ((alias("_ZN4EPOS1S2IC18software_interruptEv"))); }
extern "C" { void _prefetch_abort() __attribute__ ((alias("_ZN4EPOS1S2IC14prefetch_abortEv"))); }
extern "C" { void _sync_exception() __attribute__ ((alias("_ZN4EPOS1S2IC10data_abortEv"))); }
extern "C" { void _reserved() __attribute__ ((alias("_ZN4EPOS1S2IC8reservedEv"))); }
extern "C" { void _fiq() __attribute__ ((alias("_ZN4EPOS1S2IC3fiqEv"))); }

__BEGIN_SYS

// Class attributes
IC::Interrupt_Handler IC::_int_vector[IC::INTS];
// Class attributes
IC::Interrupt_Handler IC::_eoi_vector[INTS] = {
    0,
    Timer::eoi, // System Timer C1
    0,
    0, // System Timer C3 (User_Timer)--> should we add the EOI function here?
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    Timer::eoi, // ARM TIMER INT
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    // For the sake of the simplicity of the code, a single interrupt id is used to issue IPIs.
    // In this way, we only map a single handler, agnostic of MBOX number and Core ID.
    // On the other hand, handlers take Core ID into consideration when performing EOIs.
    // As a single MBOX handler is used to address all IPIs, we clear all MBOX of a Core in this handler.
    mailbox_eoi, // MB0_CPU0
    0,//mailbox_eoi, // MB1_CPU0
    0,//mailbox_eoi, // MB2_CPU0
    0,//mailbox_eoi, // MB3_CPU0
    0,//mailbox_eoi, // MB0_CPU1
    0,//mailbox_eoi, // MB1_CPU1
    0,//mailbox_eoi, // MB2_CPU1
    0,//mailbox_eoi, // MB3_CPU1
    0,//mailbox_eoi, // MB0_CPU2
    0,//mailbox_eoi, // MB1_CPU2
    0,//mailbox_eoi, // MB2_CPU2
    0,//mailbox_eoi, // MB3_CPU2
    0,//mailbox_eoi, // MB0_CPU3
    0,//mailbox_eoi, // MB1_CPU3
    0,//mailbox_eoi, // MB2_CPU3
    0,//mailbox_eoi, // MB3_CPU3
    Timer::eoi, // CORE0 LOCAL TIMER INT = 117
    0
};

// Class methods
void IC::entry()
{
        ASM(
        //Restore the reg used by the vector table
        "mrs x20, daif\n"
        "mov x20, #960\n"
        "msr daif, x20\n"
        "ldr  x20, [sp], #8             \n"
        //Context
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
        "mrs x0, elr_el1  \n"
        "mrs x1, spsr_el1 \n"
        "str x0, [sp,#-8]!\n"
        "str x1, [sp,#-8]!\n"
        "ldr x0, [sp,#16] \n"
        "ldr x1, [sp,#24] \n"
        //Save the context
        //Call dispatch
        "bl _dispatch                   \n"

        "mov x20, #960\n"
        "msr daif, x20\n"
        "ldr  x0, [sp], #8              \n"
        "msr spsr_el1, x0\n"
        "ldr  x0, [sp], #8              \n"
        "msr elr_el1, x0\n"
        "ldr  x0, [sp], #8              \n"
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
        // "ldr  x30, [sp], #8             \n"
        //Return
        "eret                           \n"
        : : "i"(dispatch));
}

void IC::dispatch(unsigned int i)
{
    Interrupt_Id id = int_id();

    if((id != INT_SYS_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << id << ",handler=" << hex << reinterpret_cast<void *>(_int_vector[id]) << ")" << endl;

    db<IC>(TRC) << "IC::dispatch(stack=" << CPU::sp() << ")" << endl;

    assert(id < INTS);
    if(_eoi_vector[id])
        _eoi_vector[id](id);

    CPU::int_enable();

    _int_vector[id](id);
}

void IC::eoi(unsigned int id)
{
    if((id != INT_SYS_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::eoi(i=" << id << ")" << endl;

    assert(id < INTS);
    if(_eoi_vector[id])
        _eoi_vector[id](id);
}

void IC::int_not(Interrupt_Id i)
{
    db<IC>(WRN) << "IC::int_not(i=" << i << ")" << endl;
}

void IC::hard_fault(Interrupt_Id i)
{
    db<IC>(ERR) << "IC::hard_fault(i=" << i << ")" << endl;
    // Machine::panic();
    __exit();
}

void IC::undefined_instruction()
{
    db<IC>(ERR) << "SError exception" << endl;
    // Machine::panic();
    __exit();
}

void IC::software_interrupt()
{
    db<IC>(ERR) << "Software interrupt" << endl;
    // Machine::panic();
    __exit();
}

void IC::prefetch_abort()
{
    db<IC>(ERR) << "Prefetch abort" << endl;
    // Machine::panic();
    __exit();
}

void IC::data_abort()
{
    ASM(
        //Restore the reg used by the vector table
        "mrs x20, esr_el1\n"
        "lsr x20, x20, #26\n"
        "cmp x20, #21\n"
        "bne .end_le\n"
        "mrs x20, daif\n"
        "mov x20, #960\n"
        "msr daif, x20\n"
        "ldr  x20, [sp], #8             \n"
        //Context
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
        "mrs x0, elr_el1  \n"
        "mrs x1, spsr_el1 \n"
        "str x0, [sp,#-8]!\n"
        "str x1, [sp,#-8]!\n"
        "ldr x0, [sp,#16] \n"
        "ldr x1, [sp,#24] \n"
        //Save the context
        //Call dispatch
        "bl %0                   \n"

        "mov x20, #960\n"
        "msr daif, x20\n"
        "ldr  x0, [sp], #8              \n"
        "msr spsr_el1, x0\n"
        "ldr  x0, [sp], #8              \n"
        "msr elr_el1, x0\n"
        "ldr  x0, [sp], #8              \n"
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
        // "ldr  x30, [sp], #8             \n"
        //Return
        "eret                           \n"
        ".end_le:\n"
        "ldr  x20, [sp], #8             \n"
        : : "i"(CPU::syscalled));
        __exit();
    // CPU::syscalled()
    // db<IC>(ERR) << "Sync exception" << endl;
    // Machine::panic();
}

void IC::reserved()
{
    db<IC>(ERR) << "Reserved" << endl;
    __exit();
    // Machine::panic();
}

void IC::fiq()
{
    db<IC>(ERR) << "FIQ handler" << endl;
    __exit();
    //Machine::panic();
}

__END_SYS
