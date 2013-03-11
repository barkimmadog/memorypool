#ifdef CX_MEMORY_H

class Object {
	int x;
public:
	int getX(void) { return x; };
	void setX(int fx) { x = fx; };

	void* operator new(size_t size) { return Memory::Instance()->Allocate(size);
	}
	void operator delete(void* ptr) { Memory::Instance()->Free(ptr,
	sizeof(Object)); }
};

#endif
