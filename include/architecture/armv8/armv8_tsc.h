// EPOS ARMv8 Time-Stamp Counter Mediator Declarations

#ifndef __armv8_tsc_h
#define __armv8_tsc_h

#include <architecture/cpu.h>
#include <architecture/tsc.h>
#include <system/memory_map.h>

#define __ic_common_only__
#include <machine/ic.h>
#undef __ic_common_only__

__BEGIN_SYS

class TSC: private TSC_Common
{
    friend class CPU;
    friend class IC;

private:
    static const Hertz CLOCK = Traits<Build>::MODEL == Traits<Build>::Raspberry_Pi3 ? 1000000
                             : Traits<Build>::MODEL == Traits<Build>::Zynq ? Traits<CPU>::CLOCK / 2
                             : Traits<CPU>::CLOCK;
    static const PPB ACCURACY = 40000; // ppb

    // Cortex-M3 GPTM registers offsets
    enum {              // Description
        GPTMTAR = 0x48, // Counter
    };

    // Zynq Global Timer Registers offsets
    enum {             // Description
        GTCTRL = 0x00, // Low Counter
        GTCTRH = 0x04, // High Counter
        GTCLR  = 0x08, // Control
        GTISR  = 0x0C  // Interrupt Status
    };

    // BCM2835 cortex-a53 System Timer
    enum {                                      // Description
        STCS                        = 0x00,     // Control/Status
        STCLO                       = 0x04,     // Low COUNTER
        STCHI                       = 0x08,     // High Counter
        STC0                        = 0x0C,     // Compare 0 - Used by GPU
        STC1                        = 0x10,     // Compare 1 - Value used to generate interrupt 1
        STC2                        = 0X14,     // Compare 2 - Used by GPU
        STC3                        = 0X18      // Compare 3 - Value used to generate interrupt 3
        // Interrupts mapped to "Enable IRQ 1" - c1 and c3 == irq1 and irq3
    };

public:
    using TSC_Common::Time_Stamp;

    static const unsigned int FREQUENCY = CLOCK;

public:
    TSC() {}

    static Hertz frequency() { return CLOCK; }
    static PPB accuracy() { return ACCURACY; }

    static Time_Stamp time_stamp() {
        return reg(STCLO);
    }

private:
    static void init();

    static volatile CPU::Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile CPU::Reg32 *>(Memory_Map::TSC_BASE)[o / sizeof(CPU::Reg32)]; }


};

__END_SYS

#endif
