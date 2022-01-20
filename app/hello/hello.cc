#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "Hello world!" << endl;
    int a = 1;
    int b = 5;
    int c = a + b;
    cout << "Soma a + b = " << c << endl;
    int * d = (int*) malloc(50);
    cout << "a address = " << d << endl;
    free(d);

    return 0;
}
