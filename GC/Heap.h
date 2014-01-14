/*
 * Heap.h
 *
 *  Created on: Nov 16, 2013
 *      Author: Christoph Zimprich, Stefan Anzinger
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

/**
 * @brief returns the pointer to the typedescriptor
 */
#define TYPE_DESCRIPTOR(block) ((int64_t*)((block)->used.typeTag.scal&~3))
/**
 * @brief returns true, if the block is used
 */
#define IS_USED(block) ((block)->used.typeTag.scal&1)
/**
 * @brief returns true, if the block is marked
 */
#define IS_MARKED(block) ((block)->typeTag.scal&2)
/**
 * @brief Return the block length, either the block is used or free.
 */
#define BLOCK_LENGTH(block) (IS_USED(block) ? TYPE_DESCRIPTOR(block)[0] : block->free.length)
/**
 * @brief return the next block
 */
#define NEXT_BLOCK(block) ((Block*)((uint64_t)block+BLOCK_LENGTH(block)))
/**
 * @brief add scalar value to the type tag.
 */
#define ADD_TO_TAG(block, by) (block->typeTag.scal += by)
/**
 * @brief set the mark bit.
 */
#define MARK(block) (block->typeTag.scal |= 2)
/**
 * @brief Remove the mark bit.
 */
#define UNMARK(block) (block->typeTag.scal &= ~2)
/**
 * @brief changes a object to a block pointer
 */
#define OBJECT_TO_BLOCK(obj) ((UsedBlock*)((uint64_t)obj-HEAP_POINTER_LENGTH))
/**
 * @brief Changes a block pointer to a object pointer
 */
#define BLOCK_TO_OBJECT(obj) ((UsedBlock*)((uint64_t)obj+HEAP_POINTER_LENGTH))
/**
 * @brief Get pointer address by byte offset
 */
#define POINTER_ADDRESS(block, off) (void**)(&block->data+off/HEAP_POINTER_LENGTH)
/**
 * @brief get the type tag.
 */
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
public:
	uint64_t allocated;
	uint64_t freed;
	uint64_t gcd;
	uint64_t merged;
private:
	list<TypeDescriptor*> *typeDescriptors;
	list<void*> *roots;
	/**
	 * @brief returns the typedescriptor object by typename
	 */
	TypeDescriptor* getByName(string name);
	/**
	 * @brief initializes the heap
	 */
	void initHeap();
	/**
	 * @brief splits the given block at position n
	 */
	FreeBlock *splitBlock(FreeBlock *block, uint64_t n);
	/**
	 * @brief Helper function, which just checks whether the block seems corrupt, if so, it prints an error message to stdout
	 */
	bool validateFreeBlock(FreeBlock *block);
	/**
	 * @brief merges two free blocks
	 */
	void merge(FreeBlock *a, FreeBlock *b);
	/**
	 * @brief implements the first fit strategy for finding an appropriate block
	 */
	FreeBlock* findBlockWithMinSize(uint64_t size);
	/**
	 * @brief Removes the given block from the free list
	 */
	void useBlock(FreeBlock* b);
	/**
	 * @brief Frees the block and if possible do a merge of neighboring blocks
	 */
	void freeBlock(UsedBlock* block, FreeBlock* next);
	/**
	 * @brief Sets tye type tag to the object block
	 */
	void setTypeTag(FreeBlock* b, TypeDescriptor* desc);
	/**
	 * @brief Marks the block as seen during the mark phase
	 */
	void markBlock(UsedBlock* rootBlock);
	/**
	 * @brief Implements the sweep phase
	 */
	void sweep();
	/**
	 * @brief Helper function, which just makes sure, the type tag contains a plausible value
	 */
	bool validateTypeTag(UsedBlock* b);
	/**
	 * @brief Returns the type descriptor with the by a block
	 */
	TypeDescriptor* getByBlock(UsedBlock* b);
	void validateFirstFreeBlock();
public:
	Heap();
	virtual ~Heap();
	/**
	 * @brief allocate memory for the type.
	 */
	void* alloc(string typeId);
	/**
	 * @brief initiates the gc
	 */
	void gc();
	/**
	 * @brief add to type registry
	 */
	void registerType(TypeDescriptor *descriptor);
	/**
	 * @brief add root pointer
	 */
	void addRoot(void* root);
	/**
	 * @brief remove root pointer
	 */
	void removeRoot(void* root);
	/**
	 * @brief return the amount of free bytes on heap
	 */
	uint64_t getFreeBytes();
	/**
	 * @brief dump the heap to stdout
	 */
	void dumpHeap();
};


#endif /* HEAP_H_ */
