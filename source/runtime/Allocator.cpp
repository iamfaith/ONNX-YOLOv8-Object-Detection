#include "Allocator.h"
#include <iostream>

namespace runtime
{
    void *DefaultAllocator::fastMalloc(size_t size) 
    {
       return runtime::fastMalloc(size);
    }

    void DefaultAllocator::fastFree(void *ptr)
    {
        runtime::fastFree(ptr);
    }
}

int main()
{
    using namespace std;

    // void *test = runtime::fastMalloc(100);
    // runtime::fastFree(test);

    runtime::DefaultAllocator * allocator = new runtime::DefaultAllocator;
    void* test = allocator->fastMalloc(100);
    allocator->fastFree(test);
    cout << "hello" << endl;
}