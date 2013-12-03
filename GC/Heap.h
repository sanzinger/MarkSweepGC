/*
 * Heap.h
 *
 *  Created on: Nov 16, 2013
 *      Author: stefan
 */

#ifndef HEAP_H_
#define HEAP_H_
#include <string>
#include <list>
#include <stdint.h>
#include "TypeDescriptor.h"

using namespace std;

typedef uint64_t heap_pointer;
typedef uint32_t pointer_offset;
#define HEAP_POINTER_LENGTH 8
#define HEAP_INTEGER_LENGTH 8
#define HEAP_SENTINEL_LENGTH 8
#define HEAP_SIZE 2^10*32
#define MIN_BLOCK_SIZE 16 // Bytes
#define BLOCK_ALIGN 8 // Bytes

struct FreeBlock {
	uint64_t length;
	FreeBlock *next;
};

class Heap {
	uint8_t heap[HEAP_SIZE]; // 32 Kilobyte heap
private:
	list<TypeDescriptor*> *typeDescriptors;
	TypeDescriptor* getByName(string name);
	void initHeap();
	FreeBlock *splitBlock(FreeBlock *block, uint64_t n);
	bool validateFreeBlock(FreeBlock *block);
public:
	Heap();
	virtual ~Heap();
	void alloc(string typeId);
	void gc();
	void registerType(TypeDescriptor *descriptor);
};


#endif /* HEAP_H_ */
