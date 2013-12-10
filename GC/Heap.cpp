/*
 * Heap.cpp
 *
 *  Created on: Nov 16, 2013
 *      Author: stefan
 */

#include "Heap.h"
#include <iostream>
#include <cstdio>


using namespace std;

Heap::Heap() {
	typeDescriptors = new list<TypeDescriptor*>();
	roots = new list<uint64_t*>();
	initHeap();
}

Heap::~Heap() {
	delete typeDescriptors;
	delete roots;
}

void Heap::gc() {
	list<uint64_t*>::iterator it = roots->begin();
	list<uint64_t*>::iterator end = roots->end();

	for(;it != end; ++it) {
		markBlock((UsedBlock*)*it);
	}
	dumpHeap();
	sweep();
}

void Heap::markBlock(UsedBlock* cur) {
	UsedBlock* prev = NULL; // prev
	void** padr = NULL;
	UsedBlock* p = NULL;
	MARK(cur);

	for(;;) {
		ADD_TO_TAG(cur, HEAP_POINTER_LENGTH);
		int64_t off = GET_OFFSET(cur);

		if(off >= 0) {
			// pointer to the pointer of the next block (in current block)
			padr = POINTER_ADDRESS(cur, off);
			p = OBJECT_TO_BLOCK(*padr);
			if(*padr != NULL && !IS_MARKED(p)) {
				*padr = BLOCK_TO_OBJECT(prev);
				prev = cur;
				cur = p;
				MARK(cur);
			}
		} else {
			ADD_TO_TAG(cur, off); // Restore tag (off < 0)
			if(prev == NULL) {
				return;
			}
			p = cur;
			cur = prev;
			off = GET_OFFSET(cur);
			padr = POINTER_ADDRESS(cur, off);
			prev = OBJECT_TO_BLOCK(*padr);
			*padr = BLOCK_TO_OBJECT(p);
		}
	}
}

void Heap::sweep() {
	FreeBlock* lastFreeBlock = NULL;
	bool dataBetween = false;
	Block* b = (Block*)heap;
	while(b < (Block*)(&heap[HEAP_SIZE])) {
		UsedBlock* bUsed = &b->used;
		FreeBlock* bFree = &b->free;
		bool isUsed = IS_USED(b);
		bool isMarked = IS_MARKED(bUsed);
		if(isUsed && !isMarked) {
			freeBlock(bUsed, NULL);
		}
		if(isUsed && isMarked) {
			dataBetween = true;
			UNMARK(bUsed);
		} else {
			if(dataBetween) {
				dataBetween = false;
				if(lastFreeBlock != NULL) {
					lastFreeBlock->next = bFree;
				}
				lastFreeBlock = NULL;
			}
			if(lastFreeBlock != NULL) {
				merge(lastFreeBlock, bFree);
			} else {
				lastFreeBlock = bFree;
			}
		}
		b= NEXT_BLOCK(b);
	}
}

void Heap::freeBlock(UsedBlock* usedBlock, FreeBlock* next) {
	Block* block = (Block*)usedBlock;
	uint64_t size = *TYPE_DESCRIPTOR(block);
	usedBlock->typeTag.scal &= ~1;
	block->free.length = size;
	block->free.next = next;
	if((uint64_t)block < (uint64_t)firstFreeBlock) {
		firstFreeBlock = &block->free;
	}
}

void* Heap::alloc(string typeId) {
	TypeDescriptor* type = getByName(typeId);
	uint64_t requiredSize = type->getObjectSize();
	FreeBlock* firstFitBlock = findBlockWithMinSize(requiredSize);
	if(firstFitBlock != NULL) {
		splitBlock(firstFitBlock, requiredSize);
		useBlock(firstFitBlock);
		setTypeTag(firstFitBlock, type);
		//cout << "allocate(" << typeId << "): Required bytes: " << requiredSize << " Free after allocate: " << getFreeBytes() << endl;
		return (void*)((uint64_t)firstFitBlock+HEAP_INTEGER_LENGTH);
	} else {
		cout << "WARN: Out of memory" << endl;
		return NULL;
	}
}

void Heap::setTypeTag(FreeBlock* b, TypeDescriptor* desc) {
	uint64_t* typeTag = (uint64_t*)b;
	*typeTag = (*typeTag & 0x3);
	*typeTag |= (uint64_t)desc->getDescriptor() & (~0x3);
	cout <<"";
}

uint64_t Heap::getFreeBytes() {
	FreeBlock* i = this->firstFreeBlock;
	uint64_t bytes = 0;
	while(i != NULL) {
		bytes += i->length;
		i = i->next;
	}
	return bytes;
}

FreeBlock* Heap::findBlockWithMinSize(uint64_t size) {
	FreeBlock* i = this->firstFreeBlock;
	while(i != NULL && i->length < size) {
		i = i->next;
	}
	if(i!=NULL && i->length > size) {
		return i;
	} else {
		return NULL;
	}
}

void Heap::useBlock(FreeBlock* b) {
	FreeBlock* i = this->firstFreeBlock;
	FreeBlock* prev = NULL;
	while(i != NULL && i != b) {
		prev = i;
		i = i->next;
	}
	if(i == b) {
		if((uint64_t)i->next > ((uint64_t)heap + sizeof(heap))) {
			cout << "ERROR i->next=" << i->next << " out of heap [" << (void*)heap << ":" << (void*)((uint64_t)heap + sizeof(heap)) << "]" << endl;
		}
		this->firstFreeBlock = i->next;
	} else {
		prev->next = i->next;
	}
	// Set the used bit (bit 0) and all others to 0
	*(uint64_t*)b = 1;
}

void Heap::merge(FreeBlock *a, FreeBlock *b) {
	validateFreeBlock(a);
	validateFreeBlock(b);

	FreeBlock *temp = b->next;
	a->length += b->length;
	a->next = temp;
}

FreeBlock* Heap::splitBlock(FreeBlock *block, uint64_t n) {
	if(n % BLOCK_ALIGN != 0) {
		cout << "splitBlock(" << block << ", " << n << "): n is not aligned by " << BLOCK_ALIGN << " bytes." << endl;
		return NULL;
	}
	if(block->length < n || n < MIN_BLOCK_SIZE || block->length - n < MIN_BLOCK_SIZE) {
		cout << "splitBlock(" << block << ", " << n << "): requested split violates MIN_BLOCK_SIZE=" <<
				MIN_BLOCK_SIZE << " or exceeds block length of " << block->length - MIN_BLOCK_SIZE << endl;
		return NULL;
	}
	validateFreeBlock(block);
	FreeBlock *newBlock = (FreeBlock*)(((uint64_t)block)+n);
	newBlock->next = block->next;
	block->next = newBlock;
	newBlock->length = block->length-n;
	block->length = n;
	validateFreeBlock(newBlock);
	return newBlock;
}

bool Heap::validateFreeBlock(FreeBlock *block) {
	bool result = (*((uint64_t*)block) & 0x1) == 0;
	if(!result) {
		cout << "Detected invalid free block at position " << block << endl;
	}
	return result;
}

void Heap::registerType(TypeDescriptor *typeDescriptor) {
	typeDescriptors->push_back(typeDescriptor);
}

void Heap::dumpHeap() {
	cout << "Heapdump:" << endl;
	Block* b = (Block*)heap;
	while(b < (Block*)(&heap[HEAP_SIZE])) {
		bool isUsed = IS_USED(b);
		std::fprintf(stdout, "    %p(%3ld): ", b, BLOCK_LENGTH(b));
		cout << "isUsed=" << isUsed << " ";
		if(isUsed) {
			bool isMarked = IS_MARKED((&b->used));
			cout << " isMarked=" << isMarked << " ";
			if(validateTypeTag(&b->used)) { // If typetag is valid
				TypeDescriptor* td = getByBlock(&b->used);
				cout << td->getName();
			} else {
				cout << endl << "ERROR: Invalid TypeTag, aborting." << endl;
				return;
			}
		} else {
			cout << "<FreeBlock> " << " next=" << b->free.next << " ";
		}
		b= NEXT_BLOCK(b);
		cout << endl;
	}
}

TypeDescriptor* Heap::getByBlock(UsedBlock* b) {
	list<TypeDescriptor*>::iterator it = typeDescriptors->begin();
	list<TypeDescriptor*>::iterator end = typeDescriptors->end();

	for(;it != end; ++it) {
		if((*it)->getDescriptor() == (void*)(b->typeTag.scal&~0x3)) {
			return *it;
		}
	}
	return NULL;
}

bool Heap::validateTypeTag(UsedBlock* b) {
	int64_t* typeTag = (int64_t*)(b->typeTag.scal & ~0x3);
	bool error = false;
	if(!(b->typeTag.scal & 1)) {
		cout << "ERROR: Type tag of object " << b << " is not set as used" << endl;
		error = true;
	}
	int64_t idx = -1;
	while(typeTag[++idx] >= 0);
	if(-idx*HEAP_INTEGER_LENGTH != typeTag[idx]) {
		cout << "ERROR: Pointer to TypeDescriptor of UsedBlock " << b << " seems to be wrong. Expected sentinel " <<
				-idx << " and real " << typeTag[idx] << endl;
		error = true;
	}
	return !error;
}

void Heap::initHeap() {
	FreeBlock *rootBlock = (FreeBlock*)heap;
	rootBlock->length = HEAP_SIZE;
	rootBlock->next = NULL;
	this->firstFreeBlock = rootBlock;
}

TypeDescriptor* Heap::getByName(string name) {
	list<TypeDescriptor*>::iterator it = typeDescriptors->begin();
	list<TypeDescriptor*>::iterator end = typeDescriptors->end();
	for(;it != end; ++it) {
		if((*it)->getName() == name) {
			return *it;
		}
	}
	return NULL;
}

void Heap::addRoot(uint64_t* root) {
	roots->push_back(root-1);
}
