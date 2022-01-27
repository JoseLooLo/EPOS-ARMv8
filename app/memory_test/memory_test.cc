#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

static const unsigned int HEAP_SIZE = Traits<Application>::HEAP_SIZE;

void end_allocation_test(void) {
    cout << "Starting end allocation test..." << endl;

    int block_size = HEAP_SIZE / 5;
    cout << "Block size: " << block_size << endl;

    cout << "Allocating A with "<< block_size << " bytes..." << endl;
    void * a = malloc(block_size);
    cout << "A was allocated at the address " << a << endl;
    cout << "Allocating B with "<< block_size << " bytes..." << endl;
    void * b = malloc(block_size);
    cout << "B was allocated at the address " << b << endl;
    cout << "Allocating C with "<< block_size << " bytes..." << endl;
    void * c = malloc(block_size);
    cout << "C was allocated at the address " << c << endl;

    cout << "Starting asserts end allocation test..." << endl;
    assert(a > b);
    cout << "B was allocated before A. pass [1/3]" << endl;
    assert(b > c);
    cout << "C was allocated before B. pass [2/3]" << endl;

    cout << "Deallocating B..." << endl;
    free(b);

    cout << "================================" << endl;
    cout << "In this point we have 2 availables spaces on the heap" << endl;
    cout << "The space between A and C and the end of the heap" << endl;
    cout << "The space between A and C should have " << block_size << " bytes" << endl;
    cout << "The end of the heap should have " << 2*block_size << "bytes" << endl;
    cout << "If we alloc a new space, this space must go to the end of the heap" << endl;
    cout << "================================" << endl;

    cout << "Allocating D with 100 bytes..." << endl;
    void * d = malloc(100);
    cout << "D was allocated at the address " << d << endl;

    assert(c > d);
    cout << "D was allocated before C. pass [3/3]" << endl;

    cout << "Deallocating A..." << endl;
    free(a);
    cout << "Deallocating C..." << endl;
    free(c);
    cout << "Deallocating D..." << endl;
    free(d);
    cout << "Finishing the end allocation test" << endl;
}

void middle_allocation_test(void) {
    cout << "Starting middle allocation test..." << endl;

    int block_size = HEAP_SIZE / 7;
    cout << "Block size: " << block_size << endl;

    cout << "Allocating A with "<< 2*block_size << " bytes..." << endl;
    void * a = malloc(2*block_size);
    cout << "A was allocated at the address " << a << endl;
    cout << "Allocating B with "<< 2*block_size << " bytes..." << endl;
    void * b = malloc(2*block_size);
    cout << "B was allocated at the address " << b << endl;
    cout << "Allocating C with "<< 2*block_size << " bytes..." << endl;
    void * c = malloc(2*block_size);
    cout << "C was allocated at the address " << c << endl;

    cout << "Starting asserts middle allocation test..." << endl;
    assert(a > b);
    cout << "B was allocated before A. pass [1/4]" << endl;
    assert(b > c);
    cout << "C was allocated before B. pass [2/4]" << endl;

    cout << "Deallocating B..." << endl;
    free(b);

    cout << "================================" << endl;
    cout << "In this point we have 2 availables spaces on the heap" << endl;
    cout << "The space between A and C and the end of the heap" << endl;
    cout << "The space between A and C should have " << 2*block_size << " bytes" << endl;
    cout << "The end of the heap should have " << block_size << "bytes" << endl;
    cout << "If we alloc a new space, this space must go between A and C" << endl;
    cout << "================================" << endl;

    cout << "Allocating D with 100 bytes..." << endl;
    void * d = malloc(100);
    cout << "D was allocated at the address " << d << endl;

    assert(a > d);
    cout << "D was allocated before A. pass [3/4]" << endl;
    assert(c < d);
    cout << "D was allocated after C. pass [4/4]" << endl;

    cout << "Deallocating A..." << endl;
    free(a);
    cout << "Deallocating C..." << endl;
    free(c);
    cout << "Deallocating D..." << endl;
    free(d);
    cout << "Finishing the middle allocation test" << endl;
}

int main()
{

    cout << "Group L - Worst fit heap test..." << endl;
    cout << "================================" << endl;
    cout << "For this test the traits have been changed" << endl;
    cout << "to enable explicit multiheap and with that" << endl;
    cout << "we gain more control over heap size. This" << endl;
    cout << "change makes it possible to run test over" << endl;
    cout << "heap since now we can get it sizes easily." << endl;

    cout << "================================" << endl;
    cout << "Application Heap size: " << HEAP_SIZE << endl;
    cout << "================================" << endl;

    end_allocation_test();
    middle_allocation_test();
    return 0;
}
