/*
 * Heap.cpp
 *
 *  Created on: Nov 16, 2013
 *      Author: stefan
 */

#include "Heap.h"
#include <iostream>


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
		markBlock(*it);
	}

	sweep();
}

void Heap::markBlock(uint64_t* currentBlock) {
	uint64_t* prevBlock = NULL; // prev
	uint64_t* offsetBlock = NULL; //p
	uint64_t* padr = NULL;
	*(currentBlock-1) |= 0x2; // Mark

	for(;;) {
		uint64_t* typeTagAdr = (uint64_t*)(*(currentBlock-1) & ~0x3);
		// TODO: Bad smell, resets the used bit
		*(currentBlock-1) &= 0x3;
		*(currentBlock-1) |= (uint64_t)(typeTagAdr + 1) & ~0x3;
		int64_t offset = *(typeTagAdr+1);

		if(offset >= 0) {
			cout << "advance" << endl;
			// pointer to the pointer of the next block (in current block)
			padr = (uint64_t*)((uint64_t)currentBlock+offset);
			offsetBlock = (uint64_t*)*padr; // pointer to the next block

			if(offsetBlock != NULL && (*(offsetBlock-1) & 0x2) == 0) {
				*padr = (uint64_t)prevBlock;
				prevBlock = currentBlock;
				currentBlock = offsetBlock;
				*(currentBlock-1) |= 0x2; // Mark
				cout << "Block marked" << endl;
			}
		} else {
			cout << "retreat" << endl;
			*(currentBlock-1) += offset; // restore tag
			if(prevBlock == NULL) {
				cout << "reached end" << endl;
				return;
			}
			offsetBlock = currentBlock;
			currentBlock = prevBlock;
			offset = *typeTagAdr;
			padr = (uint64_t*)((uint64_t)currentBlock + offset);
			prevBlock = (uint64_t*)*padr;
			*padr = (uint64_t)offsetBlock;
		}
	}
}

void Heap::sweep() {
	uint64_t* cur = (uint64_t*)heap;
	uint64_t* heapEnd = (uint64_t*)(heap+HEAP_SIZE);

	FreeBlock* lastFreeBlock = NULL;
	bool dataBetween = false;

	cout << cur <<" "<<heapEnd << endl;
	while(cur < heapEnd) {
		bool isFree = !(*cur & 0x1);
		bool isUnmarked = !(*cur & 0x2);
		cout << "Block(" << (uint64_t)cur-(uint64_t)heap << "): isFree=" << isFree << ", isUnmarked=" << isUnmarked;
		if(!isFree && isUnmarked) {
			freeBlock(cur, NULL);
			cout << "free Block";
		}

		if(isFree || isUnmarked) {
			if(dataBetween) {
				dataBetween = false;
				if(lastFreeBlock != NULL) {
					lastFreeBlock->next = (FreeBlock*)cur;
				}
				lastFreeBlock = NULL;
			}

			if(lastFreeBlock != NULL) {
				merge(lastFreeBlock, (FreeBlock*)cur);
			} else {
				lastFreeBlock = (FreeBlock*)cur;
			}

			cur = (uint64_t*)((uint64_t)cur+8+*cur);
		} else {
			dataBetween = true;
			uint64_t* typeTagAdr = (uint64_t*)(*cur & ~0x3);
			cur = (uint64_t*)((uint64_t)cur+8+*typeTagAdr);
		}
		cout << endl;
	}
}

void Heap::freeBlock(uint64_t* block, FreeBlock* next) {
	uint64_t* typeTagAdr = (uint64_t*)(*block & ~0x3);
	int64_t size = *typeTagAdr;

	*block = size & ~0x3;
	*(block+1) = (uint64_t)next;
	if(block < (uint64_t*)firstFreeBlock) {
		firstFreeBlock = (FreeBlock*)block;
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
		cout << "allocate(" << typeId << "): Required bytes: " << requiredSize << " Free after allocate: " << getFreeBytes() << endl;
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
	a->length += MIN_BLOCK_SIZE + b->length;
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
		bool isUsed = b->used.typeTag.scal & 1;
		bool isMarked = b->used.typeTag.scal & 2;
		cout << "  " << b << ": isUsed=" << isUsed << " isMarked=" << isMarked << " ";
		if(isUsed) {
			if(validateTypeTag(&b->used)) { // If typetag is valid
				TypeDescriptor* td = getByBlock(&b->used);
				cout << td->getName();
			} else {
				cout << endl << "ERROR: Invalid TypeTag, aborting." << endl;
				return;
			}
		} else {
			cout << "<FreeBlock> " << " next=" << b->free.next << " length=" << b->free.length;
		}
		b= (Block*)((uint64_t)b+BLOCK_LENGTH(b));
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
	roots->push_back(root);
}
