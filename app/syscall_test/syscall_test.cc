#include <utility/ostream.h>
#include <synchronizer.h>

using namespace EPOS;

OStream cout;
Semaphore k(0);
int teste2;

int teste() {
    // k.lock();
    teste2 = 5;
    cout << "teste " << teste2 << endl;
    cout << "TESTE" << endl;
    k.v();
    return 0;
}

int main()
{
    Thread * cons = new Thread(&teste);
    k.p();
    Alarm::delay(1000000);
    cout << "Hello world!" << endl;
    int a = 1;
    int b = 5;
    int c = a + b;
    cout << "Soma a + b = " << c << endl;
    int * d = (int*) malloc(50);
    cout << "a address = " << &d << endl;
    free(d);
    cons->join();
    return 0;
}
