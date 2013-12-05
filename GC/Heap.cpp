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
	initHeap();
}

Heap::~Heap() {
	delete typeDescriptors;
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
	*typeTag |= (uint64_t)desc->getDescriptor() & ~0x3;
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
	// Test if they are really neighbors?
	// Do the merge
}

FreeBlock* Heap::splitBlock(FreeBlock *block, uint64_t n) {
	if(n % BLOCK_ALIGN != 0) {
		cout << "splitBlock(" << block << ", " << n << "): n is not aligned by " << BLOCK_ALIGN << " bytes." << endl;
		return NULL;
	}
	if(block->length < n || n < MIN_BLOCK_SIZE || block->length - n < MIN_BLOCK_SIZE) {
		cout << "splitBlock(" << block << ", " << n << "): requested split violates MIN_BLOCK_SIZE=" <<
				MIN_BLOCK_SIZE << " or exceeds block length of " << block->length << endl;
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
	/**list<TypeDescriptor*>::iterator it = typeDescriptors->begin();
	list<TypeDescriptor*>::iterator end = typeDescriptors->end();
	cout << "Register: " << (typeDescriptor->getName()) << endl;
	cout << "Registered so far: " << endl;
	for(;it != end; ++it) {
		cout << " " << (*it)->getName() << endl;
	}*/
	//cout << typeDescri;
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
