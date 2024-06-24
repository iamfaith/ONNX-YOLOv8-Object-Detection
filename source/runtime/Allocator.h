
#include <stdlib.h>

namespace runtime
{

// the alignment of all the allocated buffers
// #if RUNTIME_AVX512
// #define RUNTIME_MALLOC_ALIGN 64
// #elif RUNTIME_AVX
// #define RUNTIME_MALLOC_ALIGN 32
// #else
#define RUNTIME_MALLOC_ALIGN 16
// #endif

// we have some optimized kernels that may overread buffer a bit in loop
// it is common to interleave next-loop data load with arithmetic instructions
// allocating more bytes keeps us safe from SEGV_ACCERR failure
#define RUNTIME_MALLOC_OVERREAD 64

    template <typename _Tp>
    static inline _Tp *alignPtr(_Tp *ptr, int n = (int)sizeof(_Tp))
    {
        return (_Tp *)(((size_t)ptr + n - 1) & -n);
    }

    // Aligns a buffer size to the specified number of bytes
    // The function returns the minimum number that is greater or equal to sz and is divisible by n
    // sz Buffer size to align
    // n Alignment size that must be a power of two
    static inline size_t alignSize(size_t sz, int n)
    {
        return (sz + n - 1) & -n;
    }

    static inline void *fastMalloc(size_t size)
    {
        // #if _MSC_VER
        //     return _aligned_malloc(size, RUNTIME_MALLOC_ALIGN);
        // #elif (defined(__unix__) || defined(__APPLE__)) && _POSIX_C_SOURCE >= 200112L || (__ANDROID__ && __ANDROID_API__ >= 17)
        //     void* ptr = 0;
        //     if (posix_memalign(&ptr, RUNTIME_MALLOC_ALIGN, size + RUNTIME_MALLOC_OVERREAD))
        //         ptr = 0;
        //     return ptr;
        // #elif __ANDROID__ && __ANDROID_API__ < 17
        //     return memalign(RUNTIME_MALLOC_ALIGN, size + RUNTIME_MALLOC_OVERREAD);
        // #else
        unsigned char *udata = (unsigned char *)malloc(size + sizeof(void *) + RUNTIME_MALLOC_ALIGN + RUNTIME_MALLOC_OVERREAD);
        if (!udata)
            return 0;
        unsigned char **adata = alignPtr((unsigned char **)udata + 1, RUNTIME_MALLOC_ALIGN);
        adata[-1] = udata;
        return adata;
        // #endif
    }

    static inline void fastFree(void *ptr)
    {
        if (ptr)
        {
            // #if _MSC_VER
            //         _aligned_free(ptr);
            // #elif (defined(__unix__) || defined(__APPLE__)) && _POSIX_C_SOURCE >= 200112L || (__ANDROID__ && __ANDROID_API__ >= 17)
            //         free(ptr);
            // #elif __ANDROID__ && __ANDROID_API__ < 17
            //         free(ptr);
            // #else
            unsigned char *udata = ((unsigned char **)ptr)[-1];
            free(udata);
            // #endif
        }
    }

    class Allocator
    {
    public:
        virtual ~Allocator();
        virtual void *fastMalloc(size_t size) = 0;
        virtual void fastFree(void *ptr) = 0;
    };

    class DefaultAllocator
    {
    public:
        
        virtual void *fastMalloc(size_t size);
        virtual void fastFree(void *ptr);
    };
}