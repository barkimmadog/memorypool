#ifndef CX_MEMORY_H
#define CX_MEMORY_H

#define CXPOOLSIZE 1024
#include <map>
#include <queue>
#include <cstdlib>

/*
The Memory class is a singleton that contains memory pools for objects
using it to allocate memory. The memory pools are categorized by the size
of the data being allocated.

Memory can be allocated simply by calling Allocate with a respective size. The
function returns an appropriate void pointer that has low risk of instigating
a segmentation fault. (Unless of course, the pools themselves are deleted before
program termination)

Usage:

void* operator new(size_t size) { return Memory::Instance()->Allocate(size); }
void operator delete(void* ptr) { Memory::Instance()->Free(ptr, sizeof(foo)); }

With a macro, you can shorten it with an uncommon character, such as $
#define $(x) x::Instance()
plus it looks nicer. ;)

void* operator new(size_t size) { return $(Memory)->Allocate(size); }
void operator delete(void* ptr) { $(Memory)->Free(ptr, sizeof(foo)); }

Note: this class does NOT keep track of any used pointers, it only keeps track
of available pointers.

*/

class Memory {
public:
    static Memory* Instance();
    void* Allocate(size_t);
    void  Free(void*, size_t);

private:
    Memory() {};
    Memory(Memory const&) {};
    Memory& operator=(Memory const&) { return *pInstance; };
    ~Memory();
    static Memory* pInstance;

    struct Pool {
        void* pool;
        Pool* next;
    };

    std::map<size_t, Pool> poolList; //contains list of memory pool heads
    std::map<size_t, Pool>::iterator poolListIter;
    std::map<size_t, std::deque<void*> > chunkFree; //available pointers
    std::map<size_t, std::deque<void*> >::iterator chunkFreeIter;

    //Returns a void pointer to a memory pool declared by malloc(). The pointer
    //is automatically added to the end of the linked poolList. This function
    //operates under the assumption that the Pool map already contains an entry
    //for the respective size type.
    void* ExpandPool(size_t, size_t);

    //Pushes a list of void pointers into the respective free queue based on
    //sizetype. The pointers are generated based on location of the provided
    //pool, the size of its members, and the size of the pool
    void  SegmentPool(void*, size_t, size_t);

//    #ifdef CXDEBUG
    void WriteToFile(const char* fszFilename);
//    #endif

    //Releases all memory allocated, including STL containers, and the pools
    //declared by malloc during the lifetime of this class.
    void  Cleanup();
    void  Cleanup(size_t);  // TODO class support to clear specific pools...
};

#endif //CX_MEMORY_H
