// EPOS Cortex-A Interrupt Controller Initialization

#include <architecture/cpu.h>
#include <machine/ic.h>
#include <system/memory_map.h>

__BEGIN_SYS

extern "C" { void _int_entry();
             void _serror_exception();
             void _software_interrupt();
             void _prefetch_abort();
             void _sync_exception();
             void _reserved();
             void _fiq();
}

// Class methods
void IC::init()
{
    db<Init, IC>(TRC) << "IC::init()" << endl;

    CPU::int_disable(); // will be reenabled at Thread::init() by Context::load()
    Engine::init();

    disable(); // will be enabled on demand as handlers are registered

    CPU::FSR * vt = reinterpret_cast<CPU::FSR *>(CPU::vbar_el1() + 2048);

    vt[0] = _sync_exception;
    vt[1] = _int_entry;
    vt[2] = _fiq;
    vt[3] = _serror_exception;

    // Set all interrupt handlers to int_not()
    for(Interrupt_Id i = 0; i < INTS; i++)
        _int_vector[i] = int_not;
}

__END_SYS
