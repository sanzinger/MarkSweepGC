/*
 * TypeDescriptor.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: stefan
 */

#include "TypeDescriptor.h"
#include "Heap.h"
#include <iostream>

TypeDescriptor::TypeDescriptor(string name) {
	this->name = name;
	this->descPosition = (uint64_t*)desc;
	*this->descPosition = HEAP_INTEGER_LENGTH;
	this->descPosition++;
}

uint64_t TypeDescriptor::getObjectSize() {
	return *((uint64_t*)desc);
}

void TypeDescriptor::addPointer(string name) {
	*this->descPosition = this->getObjectSize() - HEAP_INTEGER_LENGTH;
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
	// The sentinel value points points back to the beginning of the type desc block.
	// This pointer is relative, hence negative
	int64_t sentinelValue = (int64_t)((int8_t*)this->desc - (int8_t*)this->descPosition);
	*(int64_t*)this->descPosition = sentinelValue;
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


