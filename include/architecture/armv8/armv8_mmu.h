// EPOS ARMv8 MMU Mediator Declarations

#ifndef __armv8_mmu_h
#define __armv8_mmu_h

#include <architecture/mmu.h>
#include <system/memory_map.h>

__BEGIN_SYS

class ARMv8_MMU: public MMU_Common<13, 13, 16>
{
    friend class CPU;
    friend class Setup;

private:
    typedef Grouping_List<Frame> List;

    static const bool colorful = Traits<MMU>::colorful;
    static const unsigned int COLORS = Traits<MMU>::COLORS;
    static const unsigned long RAM_BASE = Memory_Map::RAM_BASE;
    static const unsigned long APP_LOW = Memory_Map::APP_LOW;
    static const unsigned long PHY_MEM = Memory_Map::PHY_MEM;
    static const unsigned long SYS = Memory_Map::SYS;

public:
    // Page Flags
    class Page_Flags
    {
    public:
        // Page Table entry flags
        enum {
            PRESENT     = 1l << 0,
            VALID       = 1l << 1,
            PT_NS       = 1l << 5,
            PT_RO_USR   = 3l << 6,              //Read only (User and kernel)
            PT_RO_KER   = 2l << 6,              //Read only (Only kernel)
            PT_RW_USR   = 1l << 6,              //ReadWrite (User and kernel)
            PT_RW_KER   = 0l << 6,              //ReadWrite (Only kernel)
            PT_RW       = 0l << 7,              //ReadWrite
            PT_RO       = 1l << 7,              //ReadOnly
            PT_USER     = 1l << 6,              //User only
            PT_KERNEL   = 0l << 6,              //Kernel only
            PT_SH_NO    = 0l << 8,              //No Shareable
            PT_SH_O     = 2l << 8,              //Outer Shareable
            PT_SH_I     = 3l << 8,              //Inner Shareable
            PT_AF       = 1l << 10,             //Acess flag
            PT_NG       = 1l << 11,             //Non Global
            PT_DBM      = 1l << 51,             //Dirty Bit
            PT_CONT     = 1l << 52,             //Continguous
            PT_PXN      = 1l << 53,             //Privileged execute-never
            PT_UXN      = 1l << 54,             //Unprivileged execute-never
            PT_EXN      = (PT_UXN | PT_PXN),     //execute-never
            PTE_FLAGS   = (PRESENT | VALID | PT_AF),
        };
        //Page Directory entry flags
        enum {
            PDE_FLAGS   = (PRESENT | VALID),
            PD_PXN      = 1l << 59,             //Privileged execute-never
            PD_UXN      = 1l << 60,             //Unprivileged execute-never
            PD_RO_USR   = 3l << 61,             //Read only (User and kernel)
            PD_RO_KER   = 2l << 61,             //Read only (Only kernel)
            PD_RW_USR   = 1l << 61,             //ReadWrite (User and kernel)
            PD_RW_KER   = 0l << 61,             //ReadWrite (Only kernel)
            PD_NS       = 1l << 63,
        };

        enum {
            //TODO
            //PT_RW_USR to IO and DMA??
            //PT_SH_I or PT_SH_O??

            //Inner shareable, User access RW
            APP  = (PT_NG | PT_SH_I | PT_RW_KER | PTE_FLAGS),
            //Inner shareable, User access RW, Execution never (User and kernel)
            APPD = (PT_NG | PT_SH_I | PT_RW_USR | PT_PXN | PT_UXN | PTE_FLAGS),
            //Inner shareable, User access RO
            APPC = (PT_NG | PT_SH_I | PT_RO_USR | PTE_FLAGS),
            //Inner shareable, Kernel access RW
            SYS  = (PT_NG | PT_SH_I | PT_RW_KER | PTE_FLAGS),
            //No shareable, Kernel access RW
            IO   = (PT_NG | PT_SH_NO | PT_RW_KER | PTE_FLAGS),
            //No shareable, Kernel access RW
            DMA  = (PT_NG | PT_SH_NO | PT_RW_KER | PTE_FLAGS),
        };

        enum {
            PT_MASK = ((PAGE_SIZE - 1)),
            PD_MASK = ((PAGE_SIZE - 1)),
        };

    public:
        Page_Flags() {}
        Page_Flags(unsigned long f) : _flags(f) {}
        Page_Flags(Flags f) : _flags(PT_NG |
                                    ((f & Flags::RW)  ? PT_RW   : PT_RO) |
                                    ((f & Flags::USR) ? PT_USER  : PT_KERNEL) |
                                    ((f & Flags::EX)  ? 0    : PT_EXN) |
                                    ((f & Flags::CT)  ? PT_CONT    : 0) |
                                    ((f & Flags::IO)  ? PT_SH_NO   : PT_SH_I) ) {}

        operator unsigned int() const { return _flags; }

        friend Debug & operator<<(Debug & db, const Page_Flags & f) { db << hex << f._flags; return db; }

    private:
        unsigned long _flags;
    };

    // Page_Table
    template<unsigned int ENTRIES>
    class _Page_Table
    {
    public:
        _Page_Table() {}

        PT_Entry & operator[](unsigned int i) { return _entry[i]; }
        _Page_Table & log() { return *static_cast<_Page_Table *>(phy2log(this)); }

        void map(int from, int to, Page_Flags flags, Color color) {
            Phy_Addr * addr = alloc(to - from, color);
            if(addr)
                remap(addr, from, to, flags);
            else
                for( ; from < to; from++) {
                    Log_Addr * pte = phy2log(&_entry[from]);
                    *pte = phy2pte(alloc(1, color), flags);
                }
        }

        void map_contiguous(int from, int to, Page_Flags flags, Color color) {
            remap(alloc(to - from, color), from, to, flags);
        }

        void remap(Phy_Addr addr, int from, int to, Page_Flags flags) {
            addr = align_page(addr);
            for( ; from < to; from++) {
                Log_Addr * pte = phy2log(&_entry[from]);
                *pte = phy2pte(addr, flags);
                addr += sizeof(Page);
            }
        }

        void unmap(int from, int to) {
            for( ; from < to; from++) {
                free(_entry[from]);
                Log_Addr * pte = phy2log(&_entry[from]);
                *pte = 0;
            }
        }

        friend OStream & operator<<(OStream & os, _Page_Table & pt) {
            os << "{\n";
            int brk = 0;
            for(unsigned int i = 0; i < ENTRIES; i++)
                if(pt[i]) {
                    os << "[" << i << "]=" << pt[i] << "  ";
                    if(!(++brk % 4))
                        os << "\n";
                }
            os << "\n}";
            return os;
        }

    private:
        PT_Entry _entry[ENTRIES]; // the Phy_Addr in each entry passed through phy2pte()
    };

    typedef _Page_Table<PT_ENTRIES> Page_Table;

    // Page Directory
    typedef _Page_Table<PD_ENTRIES> Page_Directory;

    // Chunk (for Segment)
    class Chunk
    {
    public:
        Chunk() {}

        Chunk(unsigned int bytes, Flags flags, Color color = WHITE)
        : _from(0), _to(pages(bytes)), _pts(page_tables(_to - _from)), _flags(Page_Flags(flags)), _pt(calloc(_pts, WHITE)) {
            if(!((_flags & Page_Flags::PT_CONT))) //TODO
                _pt->map_contiguous(_from, _to, _flags, color);
            else
                _pt->map(_from, _to, _flags, color);
        }

        Chunk(Phy_Addr phy_addr, unsigned int bytes, Flags flags)
        : _from(0), _to(pages(bytes)), _pts(page_tables(_to - _from)), _flags(Page_Flags(flags)), _pt(calloc(_pts, WHITE)) {
            _pt->remap(phy_addr, _from, _to, flags);
        }

        Chunk(Phy_Addr pt, unsigned long from, unsigned long to, Flags flags)
        : _from(from), _to(to), _pts(page_tables(_to - _from)), _flags(flags), _pt(pt) {}

        ~Chunk() {
            if(!(_flags & Page_Flags::IO)) {
                if(!((_flags & Page_Flags::PT_CONT))) //TODO
                    free((*_pt)[_from], _to - _from);
                else
                    for( ; _from < _to; _from++)
                        free((*_pt)[_from]);
            }
            free(_pt, _pts);
        }

        unsigned long pts() const { return _pts; }
        Page_Flags flags() const { return _flags; }
        Page_Table * pt() const { return _pt; }
        unsigned long size() const { return (_to - _from) * sizeof(Page); }

        Phy_Addr phy_address() const {
            return (!((_flags & Page_Flags::PT_CONT))) ? Phy_Addr(indexes((*_pt)[_from])) : Phy_Addr(false);
            //TODO
        }

        int resize(unsigned int amount) {
            if(!((_flags & Page_Flags::PT_CONT))) //TODO
                return 0;

            unsigned int pgs = pages(amount);

            Color color = colorful ? phy2color(_pt) : WHITE;

            unsigned int free_pgs = _pts * PT_ENTRIES - _to;
            if(free_pgs < pgs) { // resize _pt
                unsigned int pts = _pts + page_tables(pgs - free_pgs);
                Page_Table * pt = calloc(pts, color);
                memcpy(phy2log(pt), phy2log(_pt), _pts * sizeof(Page));
                free(_pt, _pts);
                _pt = pt;
                _pts = pts;
            }

            _pt->map(_to, _to + pgs, _flags, color);
            _to += pgs;

            return pgs * sizeof(Page);
        }

    private:
        unsigned long _from;
        unsigned long _to;
        unsigned long _pts;
        Page_Flags _flags;
        Page_Table * _pt; // this is a physical address
    };

    // Directory (for Address_Space)
    class Directory
    {
    public:
        Directory() : _free(true) {
            // Page Directories have 4096 32-bit entries and must be aligned to 16Kb, thus, we need 7 frame in the worst case
            Phy_Addr pd = calloc(sizeof(Page_Directory) / sizeof(Frame) + ((sizeof(Page_Directory) / sizeof(Frame)) - 1), WHITE);
            unsigned int free_frames = 0;
            while(pd & (sizeof(Page_Directory) - 1)) { // pd is not aligned to 16 Kb
                Phy_Addr * tmp = pd;
                pd += sizeof(Frame); // skip this frame
                free(tmp); // return this frame to the free list
                free_frames++;
            }
            if(free_frames != 3)
                free(pd + 4 * sizeof(Page), 3 - free_frames); // return exceeding frames at the tail to the free list

            _pd = static_cast<Page_Directory *>(pd);

            for(unsigned int i = directory(PHY_MEM); i < directory(APP_LOW); i++)
                (*_pd)[i] = (*_master)[i];

            for(unsigned int i = directory(SYS); i < PD_ENTRIES; i++)
                (*_pd)[i] = (*_master)[i];
        }

        Directory(Page_Directory * pd) : _pd(pd), _free(false) {}

        ~Directory() { if(_free) free(_pd, sizeof(Page_Directory) / sizeof(Page)); }

        Phy_Addr pd() const { return _pd; }

        void activate() const { ARMv8_MMU::pd(_pd); }

        Log_Addr attach(const Chunk & chunk, unsigned int from = directory(APP_LOW)) {
            for(unsigned int i = from; i < directory(SYS); i++)
                if(attach(i, chunk.pt(), chunk.pts(), chunk.flags()))
                    return i << DIRECTORY_SHIFT;
            return Log_Addr(false);
        }

        Log_Addr attach(const Chunk & chunk, Log_Addr addr) {
            unsigned int from = directory(addr);
            if(attach(from, chunk.pt(), chunk.pts(), chunk.flags()))
                return from << DIRECTORY_SHIFT;
            return Log_Addr(false);
        }

        void detach(const Chunk & chunk) {
            for(unsigned int i = 0; i < PD_ENTRIES; i++) {
                if(indexes(pte2phy((*_pd)[i])) == indexes(chunk.pt())) {
                    detach(i, chunk.pt(), chunk.pts());
                    return;
                }
            }
            db<MMU>(WRN) << "MMU::Directory::detach(pt=" << chunk.pt() << ") failed!" << endl;
        }

        void detach(const Chunk & chunk, Log_Addr addr) {
            unsigned int from = directory(addr);
            if(indexes(pte2phy((*_pd)[from])) != indexes(chunk.pt())) {
                db<MMU>(WRN) << "MMU::Directory::detach(pt=" << chunk.pt() << ",addr=" << addr << ") failed!" << endl;
                return;
            }
            detach(from, chunk.pt(), chunk.pts());
        }

        Phy_Addr physical(Log_Addr addr) {
            PD_Entry pde = (*_pd)[directory(addr)];
            Page_Table * pt = static_cast<Page_Table *>(pde2phy(pde));
            PT_Entry pte = pt->log()[page(addr)];
            return pte | offset(addr);
        }

    private:
        bool attach(unsigned int from, const Page_Table * pt, unsigned int n, Page_Flags flags) {
            for(unsigned int i = from; i < from + n; i++)
                if(_pd->log()[i])
                    return false;
            for(unsigned int i = from; i < from + n; i++, pt++)
                _pd->log()[i] = phy2pde(Phy_Addr(pt));
            return true;
        }

        void detach(unsigned int from, const Page_Table * pt, unsigned int n) {
            for(unsigned int i = from; i < from + n; i++) {
                _pd->log()[i] = 0;
                flush_tlb(i << DIRECTORY_SHIFT);
            }
            CPU::isb();
            CPU::dsb();
        }

    private:
        Page_Directory * _pd;  // this is a physical address, but operator*() returns a logical address
        bool _free;
    };

   // DMA_Buffer
   //Não utilizado ?
    class DMA_Buffer: public Chunk
    {
    public:
        DMA_Buffer(unsigned int s) : Chunk(s, Page_Flags::DMA) {
            Directory dir(current());
            _log_addr = dir.attach(*this);
            db<MMU>(TRC) << "MMU::DMA_Buffer() => " << *this << endl;
        }

        DMA_Buffer(unsigned int s, Log_Addr d): Chunk(s, Page_Flags::DMA) {
            Directory dir(current());
            _log_addr = dir.attach(*this);
            memcpy(_log_addr, d, s);
            db<MMU>(TRC) << "MMU::DMA_Buffer(phy=" << *this << " <= " << d << endl;
        }

        Log_Addr log_address() const { return _log_addr; }

        friend OStream & operator<<(OStream & os, const DMA_Buffer & b) {
            os << "{phy=" << b.phy_address() << ",log=" << b.log_address() << ",size=" << b.size() << ",flags=" << b.flags() << "}";
            return os;
        }

    private:
        Log_Addr _log_addr;
    };

    // Class Translation performs manual logical to physical address translations for debugging purposes only
    class Translation
    {
    public:
        Translation(Log_Addr addr, bool pt = false, Page_Directory * pd = 0): _addr(addr), _show_pt(pt), _pd(pd) {}

        friend OStream & operator<<(OStream & os, const Translation & t) {
            Page_Directory * pd = t._pd ? t._pd : current();
            PD_Entry pde = pd->log()[directory(t._addr)];
            Page_Table * pt = static_cast<Page_Table *>(pde2phy(pde));
            PT_Entry pte = pt->log()[page(t._addr)];

            os << "{addr=" << static_cast<void *>(t._addr) << ",pd=" << pd << ",pd[" << directory(t._addr) << "]=" << pde << ",pt=" << pt;
            if(t._show_pt)
                os << "=>" << pt->log();
            os << ",pt[" << page(t._addr) << "]=" << pte << ",f=" << pte2phy(pte) << ",*addr=" << hex << *static_cast<unsigned int *>(t._addr) << "}";
            return os;
        }

    private:
        Log_Addr _addr;
        bool _show_pt;
        Page_Directory * _pd;
    };

public:
    ARMv8_MMU() {}

    static Phy_Addr alloc(unsigned int frames = 1, Color color = WHITE) {
        Phy_Addr phy(false);

        if(frames) {
            List::Element * e = _free[color].search_decrementing(frames);
            if(e) {
                phy = e->object() + e->size();
                db<MMU>(TRC) << "MMU::alloc(frames=" << frames << ",color=" << color << ") => " << phy << endl;
            } else
                if(colorful)
                    db<MMU>(INF) << "MMU::alloc(frames=" << frames << ",color=" << color << ") => failed!" << endl;
                else
                    db<MMU>(WRN) << "MMU::alloc(frames=" << frames << ",color=" << color << ") => failed!" << endl;
        }

        return phy;
    }

    static Phy_Addr calloc(unsigned int frames = 1, Color color = WHITE) {
        Phy_Addr phy = alloc(frames, color);
        memset(phy2log(phy), 0, sizeof(Frame) * frames);
        return phy;
    }

    static void free(Phy_Addr frame, int n = 1) {
        // Clean up MMU flags in frame address
        frame = indexes(frame);
        Color color = colorful ? phy2color(frame) : WHITE;

        db<MMU>(TRC) << "MMU::free(frame=" << frame << ",color=" << color << ",n=" << n << ")" << endl;

        if(frame && n) {
            List::Element * e = new (phy2log(frame)) List::Element(frame, n);
            List::Element * m1, * m2;
            _free[color].insert_merging(e, &m1, &m2);
        }
    }

    static void white_free(Phy_Addr frame, int n) {
        // Clean up MMU flags in frame address
        frame = indexes(frame);

        db<MMU>(TRC) << "MMU::free(frame=" << frame << ",color=" << WHITE << ",n=" << n << ")" << endl;

        if(frame && n) {
            List::Element * e = new (phy2log(frame)) List::Element(frame, n);
            List::Element * m1, * m2;
            _free[WHITE].insert_merging(e, &m1, &m2);
        }
    }

    static unsigned int allocable(Color color = WHITE) { return _free[color].head() ? _free[color].head()->size() : 0; }

    static Page_Directory * volatile current() { return static_cast<Page_Directory * volatile>(pd());}

    static Phy_Addr physical(Log_Addr addr) {
        Page_Directory * pd = current();
        Page_Table * pt = pd->log()[directory(addr)];
        return pt->log()[page(addr)] | offset(addr);
    }

    static PT_Entry phy2pte(Phy_Addr frame, Page_Flags flags) { return (frame | flags | Page_Flags::PTE_FLAGS); }
    static Phy_Addr pte2phy(PT_Entry entry) { return (entry & ~Page_Flags::PT_MASK); }
    static PD_Entry phy2pde(Phy_Addr frame) { return (frame | Page_Flags::PDE_FLAGS); }
    static Phy_Addr pde2phy(PD_Entry entry) { return (entry  & ~Page_Flags::PD_MASK); }

    static Log_Addr phy2log(Phy_Addr phy) { return Log_Addr((RAM_BASE == PHY_MEM) ? phy : (RAM_BASE > PHY_MEM) ? phy - (RAM_BASE - PHY_MEM) : phy + (PHY_MEM - RAM_BASE)); }
    static Phy_Addr log2phy(Log_Addr log) { return Phy_Addr((RAM_BASE == PHY_MEM) ? log : (RAM_BASE > PHY_MEM) ? log + (RAM_BASE - PHY_MEM) : log - (PHY_MEM - RAM_BASE)); }

    static Color phy2color(Phy_Addr phy) { return static_cast<Color>(colorful ? ((phy >> PAGE_SHIFT) & 0x7f) % COLORS : WHITE); } // TODO: what is 0x7f

    static Color log2color(Log_Addr log) {
        if(colorful) {
            Page_Directory * pd = current();
            Page_Table * pt = (*pd)[directory(log)];
            Phy_Addr phy = (*pt)[page(log)] | offset(log);
            return static_cast<Color>(((phy >> PAGE_SHIFT) & 0x7f) % COLORS);
        } else
            return WHITE;
    }

private:
    static Phy_Addr pd() { return CPU::pd(); }
    static void pd(Phy_Addr pd) { CPU::pd(pd); CPU::flush_tlb(); CPU::isb(); CPU::dsb(); }

    static void flush_tlb() { CPU::flush_tlb(); }
    static void flush_tlb(Log_Addr addr) { CPU::flush_tlb(directory_bits(addr)); } // only bits from 31 to 12, all ASIDs

    static void init();

private:
    static List _free[colorful * COLORS + 1]; // +1 for WHITE
    static Page_Directory * _master;
};

class MMU: public IF<Traits<System>::multitask, ARMv8_MMU, No_MMU>::Result {};

__END_SYS

#endif
