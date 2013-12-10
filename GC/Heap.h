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
#define HEAP_SIZE ((1<<10)-1)*32
#define MIN_BLOCK_SIZE 16 // Bytes
#define BLOCK_ALIGN 8 // Bytes

#define TYPE_DESCRIPTOR(block) ((int64_t*)((block)->used.typeTag.scal&~3))
#define IS_USED(block) (block->used.typeTag.scal&1)
#define IS_MARKED(block) ((block)->typeTag.scal&2)
#define BLOCK_LENGTH(block) (IS_USED(block) ? TYPE_DESCRIPTOR(block)[0] : block->free.length)
#define NEXT_BLOCK(block) ((Block*)((uint64_t)block+BLOCK_LENGTH(block)))
#define ADD_TO_TAG(block, by) (block->typeTag.scal += by)
#define MARK(block) (block->typeTag.scal |= 2)
#define UNMARK(block) (block->typeTag.scal &= ~2)
#define OBJECT_TO_BLOCK(obj) ((UsedBlock*)((uint64_t)obj-HEAP_POINTER_LENGTH))
#define BLOCK_TO_OBJECT(obj) ((UsedBlock*)((uint64_t)obj+HEAP_POINTER_LENGTH))
#define POINTER_ADDRESS(block, off) (void**)(&block->data+off/HEAP_POINTER_LENGTH)
#define GET_OFFSET(block) *(int64_t*)(cur->typeTag.scal&~0x3)

union PointerOrScalar {
	int64_t* ptr;
	int64_t scal;
};

struct FreeBlock {
	uint64_t length;
	FreeBlock *next;
};

struct UsedBlock {
	PointerOrScalar typeTag;
	int64_t* data;
};

union Block {
	FreeBlock free;
	UsedBlock used;
};

class Heap {
	uint8_t heap[HEAP_SIZE]; // 32 Kilobyte heap
	FreeBlock* firstFreeBlock;
private:
	list<TypeDescriptor*> *typeDescriptors;
	list<uint64_t*> *roots;
	TypeDescriptor* getByName(string name);
	void initHeap();
	FreeBlock *splitBlock(FreeBlock *block, uint64_t n);
	bool validateFreeBlock(FreeBlock *block);
	void merge(FreeBlock *a, FreeBlock *b);
	FreeBlock* findBlockWithMinSize(uint64_t size);
	void useBlock(FreeBlock* b);
	void freeBlock(UsedBlock* block, FreeBlock* next);
	void setTypeTag(FreeBlock* b, TypeDescriptor* desc);
	void markBlock(UsedBlock* rootBlock);
	void sweep();
	bool validateTypeTag(UsedBlock* b);
	TypeDescriptor* getByBlock(UsedBlock* b);
public:
	Heap();
	virtual ~Heap();
	void* alloc(string typeId);
	void gc();
	void registerType(TypeDescriptor *descriptor);
	void addRoot(uint64_t* root);
	uint64_t getFreeBytes();
	void dumpHeap();
};


#endif /* HEAP_H_ */
