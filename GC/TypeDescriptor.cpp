/*
 * TypeDescriptor.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: stefan
 */

#include "TypeDescriptor.h"
#include "Heap.h"

TypeDescriptor::TypeDescriptor(string name) {
	this->name = name;
	this->descPosition = (uint64_t*)desc;
	*this->descPosition = 8;
	this->descPosition++;
}

uint64_t TypeDescriptor::getObjectSize() {
	return *((uint64_t*)desc);
}

void TypeDescriptor::addPointer(string name) {
	*this->descPosition = this->getObjectSize();
	this->descPosition ++;
	addToObjectSize(HEAP_POINTER_LENGTH);
}

void TypeDescriptor::addToObjectSize(uint64_t size) {
	*((uint64_t*)desc) += size;
}

void TypeDescriptor::addInteger(string name) {
	addToObjectSize(HEAP_INTEGER_LENGTH);
}

void TypeDescriptor::typeCompleted() {
	*this->descPosition = (uint64_t)(this->desc - (uint8_t*)this->descPosition);
	addToObjectSize(HEAP_SENTINEL_LENGTH);
}

string TypeDescriptor::getName() {
	return this->name;
}


void *TypeDescriptor::getDescriptor() {
	return this->desc;
}

void TypeDescriptor::addString(string name, uint64_t length) {
	// We assume that the compiler aligns the strings, pointers and integers to words, in our case it is 64 bit/8 bytes
	// Thus we align the strings as well
	length += BLOCK_ALIGN-(length%BLOCK_ALIGN);
	addToObjectSize(length);
}


