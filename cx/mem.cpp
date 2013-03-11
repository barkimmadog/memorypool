#include "mem.h"

#ifndef CX_MEMORY_C
#define CX_MEMORY_C

Memory* Memory::pInstance = NULL;

Memory::~Memory() {
    Cleanup(); //TODO verify if function is called during destructor
}

Memory* Memory::Instance() {
    if(!pInstance) pInstance = new Memory();
    return pInstance;
}

void* Memory::Allocate(size_t sizetype) {
    void* ptr = NULL;  //used to avoid declaring multiple void*'s in this function...
    //the pointer is used to initialize the pools if needed, then called again
    //to hold the pointer being used. If deque returned the pointer it was popping
    //we wouldn't need to do this! :(
    //p.s. It's not very good practice :P
    chunkFreeIter = chunkFree.find(sizetype); //check if free pointers available for sizetype

    if(chunkFreeIter == chunkFree.end()) { //if free chunk queue does not exist for size...
        //create a pool list by inserting a blank chunk
        Pool chunk;
        chunk.pool = NULL; chunk.next = NULL;
        poolList.insert(std::pair<size_t, Pool>(sizetype, chunk));
        ptr = ExpandPool(sizetype, CXPOOLSIZE);

        std::deque<void*> freequeue;
        chunkFree.insert(std::pair<size_t, std::deque<void*> >(sizetype, freequeue));

        SegmentPool(ptr, sizetype, CXPOOLSIZE); //Adds void*'s to free queue from pool
    }
    else if (chunkFreeIter->second.empty()) {
        ptr = ExpandPool(sizetype, CXPOOLSIZE);
//        if (ptr == NULL)
        SegmentPool(ptr, sizetype, CXPOOLSIZE);
    }

    ptr = (chunkFreeIter->second).front();
    (chunkFreeIter->second).pop_front(); //pop pointer from free queue
    return ptr;
}

void Memory::Free(void* ptr, size_t sizetype) {
    //previously used pointers take priority over unused ones
    chunkFreeIter = chunkFree.find(sizetype);
    chunkFreeIter->second.push_front(ptr);
}

void* Memory::ExpandPool(size_t sizetype, size_t sizepool = CXPOOLSIZE) {
    poolListIter = poolList.find(sizetype);  //TODO: assert
    Pool* pChunk = &(poolListIter->second);

    while(pChunk->next != NULL) pChunk = pChunk->next;
    pChunk->next = new Pool;
    pChunk = pChunk->next;

    pChunk->pool = malloc((sizetype+sizetype%8)*sizepool);
    pChunk->next = NULL;

    return pChunk->pool;
}

void Memory::SegmentPool(void* pool, size_t sizetype, size_t sizepool) {
    void* ptr;
    chunkFreeIter = chunkFree.find(sizetype); //chunkFreeIter->second == (queue<>)

    for(size_t ii = 0; ii < sizepool; ii++) {
        ptr = (char*) pool + (sizetype+sizetype%8) * ii;
        chunkFreeIter->second.push_back(ptr);
    }
}

void Memory::Cleanup() {
    /*
    Linked list:
    Pool.pool == void*
    Pool.next == Pool*
    */

	chunkFree.clear();

	for(poolListIter = poolList.begin(); poolListIter != poolList.end(); poolListIter++) {
        Pool* pHead  = &(poolListIter->second);
        free(pHead->pool);

        if(pHead->next != NULL) {
            Pool* pChunk = pHead->next;
            while(pChunk->next != NULL) {
                Pool* pNext = pChunk->next;
                free(pChunk->pool);
                delete pChunk;
                pChunk = pNext;
            }
            free(pChunk->pool);
            delete pChunk;
        }
	}
}

void Memory::Cleanup(size_t sizetype) {
	chunkFree.erase(sizetype);

	poolListIter = poolList.find(sizetype);
	Pool* pHead  = &(poolListIter->second);
	free(pHead->pool);

    if(pHead->next != NULL) {
        Pool* pChunk = pHead->next;
        while(pChunk->next != NULL) {
            Pool* pNext = pChunk->next;
            free(pChunk->pool); //void*
            delete pChunk;
            pChunk = pNext;
        }
        free(pChunk->pool);
        delete pChunk;
    }

	poolList.erase(poolListIter);
}

void Memory::WriteToFile(const char* fszFilename) {
//    typedef pair<int, int> position;
//    struct object
//    {
//       ushort id;
//       float x;
//       float y;
//       ushort type;
//    };
//
//    struct sector
//    {
//       ulong id;
//       ulong x;
//       ulong y;
//       object objects[256][256];
//       sector(const ulong i, const ulong xx, const ulong yy)
//          { id = i; x = xx; y = yy; }
//    };
//    int main()
//    {
//       map<position, sector> worldmap;
//
//       for(int i = 0; i < 12; i++)
//          worldmap.insert(pair<position, sector>(position(i,i+1), sector(i, i+1, i+2)));
//
//       ofstream os("world.map", ios::binary|ios::out);
//       int size = worldmap.size();
//       os.write((char*)size, sizeof(int));
//       os.write((char*)&worldmap, size * sizeof(sector));
//       os.close();
//       return 0;
//    }
}

#endif
