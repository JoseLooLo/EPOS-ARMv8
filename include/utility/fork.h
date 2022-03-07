// EPOS Fork implementation

#ifndef __fork_h
#define __fork_h

#include <process.h>
#include <architecture.h>
#include <framework/message.h>

// __BEGIN_UTIL
__USING_SYS;
extern "C" {
unsigned int fork(int(* main)()) {
    // Message msg(Id(TASK_ID, 0), Message::TASK_FORK, main);
    //     msg.act();
        return 1;

    // Task * c_task = Task::self();

    // // Code
    // Segment* cs;
    // unsigned int cs_size = c_task->code_segment()->size();
    // cs = new (SYSTEM) Segment(cs_size, Segment::Flags::APP);

    // CPU::int_disable();
    // CPU::Log_Addr tmp = c_task->address_space()->attach(cs);
    // memcpy(tmp, c_task->code(), cs_size);
    // c_task->address_space()->detach(cs);
    // CPU::int_enable();

    // // Data
    // Segment* ds;
    // unsigned int ds_size = c_task->data_segment()->size();
    // ds = new (SYSTEM) Segment(ds_size, Segment::Flags::APP);

    // CPU::int_disable();
    // CPU::Log_Addr tmp_ds = c_task->address_space()->attach(ds);
    // memcpy(tmp_ds, c_task->data(), ds_size);
    // c_task->address_space()->detach(ds);
    // CPU::int_enable();

    // typedef int (Main)();
    // main = reinterpret_cast<Main *>(main);
    // new (SYSTEM) Task(cs, ds, main, c_task->code(), c_task->data());

    // return Task::self()->task_id();
}
}

// __END_UTIL

#endif
