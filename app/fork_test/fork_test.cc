// EPOS Task Test Program

#include <time.h>
#include <process.h>
#include <machine.h>
#include <synchronizer.h>
// #include <utility/fork.h>

using namespace EPOS;

int print_inf();

OStream cout;

int main()
{
    cout << "=====Test Task Fork "<< Task::self()<<"=====" << endl;

    if (Task::self()) {
        fork(&main);
        fork(&main);
        cout << "Task: "<< Task::self() << endl;
    }
    if (Task::self()) {
        Alarm::delay(1000000);
        cout << "Task: "<< Task::self() << endl;
    }
    if (Task::self()) {
        Alarm::delay(500000);
        cout << "Task: "<< Task::self() << endl;
    }

    print_inf();
    cout << "=====End Task "<< Task::self()<<"=====" << endl;
    return 0;
}

int print_inf() {
    Task * task = Task::self();
    Address_Space * as = task->address_space();
    cout << "===========================================" << "\n"
         << "=Address Space page directory: " << as->pd() << "=\n"
         << "=Code Logical Addr: " << static_cast<void *>(task->code()) << "=\n"
         << "=Code Physical Addr: " << static_cast<void *>(as->physical(task->code())) << "=\n"
         << "=Code Size: " << task->code_segment()->size() << " bytes long" << "=\n"
         << "=Data Logical Addr: " << static_cast<void *>(task->data()) << "=\n"
         << "=Data Physical Addr: " << static_cast<void *>(as->physical(task->data())) << "=\n"
         << "=Data Size: " << task->data_segment()->size() << " bytes long" << "=\n"
         << "===========================================" << endl;
    return 0;
}
