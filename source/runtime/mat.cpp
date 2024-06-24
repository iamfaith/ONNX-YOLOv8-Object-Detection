#include "mat.h"

namespace runtime
{

    void Mat::create(int _w, int _h, int _d, int _c, size_t _elemsize, int _elempack, Allocator *_allocator)
    {
        if (dims == 4 && w == _w && h == _h && d == _d && c == _c && elemsize == _elemsize && elempack == _elempack && allocator == _allocator)
            return;
    }
}