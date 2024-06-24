
#include <stdlib.h>
#include <stddef.h>
#include "Allocator.h"
namespace runtime
{

    class Mat
    {

    public:
        Mat();

        int w;
        int h;
        int d;
        int c;
        int dims;
        size_t cstep;

        // element size in bytes
        // 4 = float32/int32
        // 2 = float16
        // 1 = int8/uint8
        // 0 = empty
        size_t elemsize;

        // packed count inside element
        // c/1-d-h-w-1  c/1-h-w-1  h/1-w-1  w/1-1  scalar
        // c/4-d-h-w-4  c/4-h-w-4  h/4-w-4  w/4-4  sse/neon
        // c/8-d-h-w-8  c/8-h-w-8  h/8-w-8  w/8-8  avx/fp16
        int elempack;

        // the allocator
        Allocator *allocator;

        void create(int w, int h, int d, int c, size_t elemsize, int elempack, Allocator *allocator = 0);
    };
}