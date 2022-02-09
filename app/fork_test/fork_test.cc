// EPOS Task Test Program

#include <time.h>
#include <process.h>
#include <machine.h>
#include <synchronizer.h>
#include <utility/fork.h>

using namespace EPOS;

int print_inf();

OStream cout;
Mutex p;

int main()
{
    cout << "=====Test Task Fork "<< Task::self()->id()<<"=====" << endl;

    if (Task::self()->id() == 0) {
        fork(&main);
        fork(&main);
        cout << "Hello World! I'm Task: "<< Task::self()->id() << endl;
        //Delay(1000000);
    }
    if (Task::self()->id() == 1) {
        Alarm::delay(10000000);
        cout << "Konnichiwa I'm Task: "<< Task::self()->id() << endl;
    }
    if (Task::self()->id() == 2) {
        Alarm::delay(5000000);
        cout << "Annyeong haseyo I'm Task: "<< Task::self()->id() << endl;
    }

    p.lock();
    print_inf();
    p.unlock();
    cout << "Sayonara, bye! o/" << endl;
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
